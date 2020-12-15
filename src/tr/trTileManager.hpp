#pragma once

#include <vector>
#include <cassert>
#include <thread>
#include <atomic>
#include "trTile.hpp"
#include "trColorBuffer.hpp"
#include "trDepthBuffer.hpp"
#include "trTriangle.hpp"

namespace tr
{
	template<typename TShader>
	class TileManager
	{
	public:
		TileManager(const size_t viewportWidth, const size_t viewportHeight, const size_t tileWidth, const size_t tileHeight) :
			m_depthTest(true),
			m_depthBias(0.0f)
		{
			setAttributes(viewportWidth, viewportHeight, tileWidth, tileHeight);
		}

		void setAttributes(const size_t viewportWidth, const size_t viewportHeight, const size_t tileWidth, const size_t tileHeight)
		{
			m_tiles.clear();

			for (size_t y = 0; y < viewportHeight; y += tileHeight)
			{
				size_t tileMaxY = y + tileHeight - 1;

				if (tileMaxY >= viewportHeight)
				{
					tileMaxY = viewportHeight - 1;
				}

				for (size_t x = 0; x < viewportWidth; x += tileWidth)
				{
					size_t tileMaxX = x + tileWidth - 1;

					if (tileMaxX >= viewportWidth)
					{
						tileMaxX = viewportWidth - 1;
					}

					m_tiles.emplace_back(Rect(x, y, tileMaxX, tileMaxY));
				}
			}
		}

		size_t addShader(const TShader& shader)
		{
			m_shaders.push_back(shader);

			return m_shaders.size() - 1;
		}

		void queue(const Triangle& triangle)
		{
			m_triangles.push_back(triangle);
		}

		void setDepthTest(const bool depthTest)
		{
			m_depthTest = depthTest;
		}

		void setDepthBias(const float depthBias)
		{
			m_depthBias = depthBias;
		}

		void clear()
		{
			m_triangles.clear();
			m_shaders.clear();
		}

		void tileThread(std::atomic<size_t>& tileIndex, ColorBuffer& colorBuffer, DepthBuffer& depthBuffer) const
		{
			size_t myTileIndex;

			while ((myTileIndex  = tileIndex.fetch_add(1, std::memory_order_relaxed)) < m_tiles.size())
			{
				const Tile& tile = m_tiles[myTileIndex];

				for (const Triangle& triangle : m_triangles)
				{
					const Rect boundingBox = triangle.boundingBox.intersection(tile.getBounds());

					if (!boundingBox.isValid())
					{
						continue;
					}

					const TShader& shader       = m_shaders[triangle.shaderIndex];

					const size_t   bufferStepX  = 4;
					const size_t   bufferStepY  = depthBuffer.getWidth() - (boundingBox.getMaxX() - boundingBox.getMinX()) + (boundingBox.getMaxX() - boundingBox.getMinX()) % bufferStepX - bufferStepX;

					Color*         colorPointer = colorBuffer.getData() + boundingBox.getMinY() * colorBuffer.getWidth() + boundingBox.getMinX();
					float*         depthPointer = depthBuffer.getData() + boundingBox.getMinY() * depthBuffer.getWidth() + boundingBox.getMinX();

					const QuadVec3 points(
						QuadFloat(float(boundingBox.getMinX()), float(boundingBox.getMinX() + 1), float(boundingBox.getMinX() + 2), float(boundingBox.getMinX() + 3)),
						QuadFloat(float(boundingBox.getMinY()), float(boundingBox.getMinY()    ), float(boundingBox.getMinY()    ), float(boundingBox.getMinY()    )),
						0.0f
					);
					
					QuadFloat rowWeights0 = orientPoints(triangle.quadVertex1.projectedPosition, triangle.quadVertex2.projectedPosition, points);
					QuadFloat rowWeights1 = orientPoints(triangle.quadVertex2.projectedPosition, triangle.quadVertex0.projectedPosition, points);
					QuadFloat rowWeights2 = orientPoints(triangle.quadVertex0.projectedPosition, triangle.quadVertex1.projectedPosition, points);

					for (size_t y = boundingBox.getMinY(); y <= boundingBox.getMaxY(); y += 1, colorPointer += bufferStepY, depthPointer += bufferStepY)
					{
						QuadFloat weights0 = rowWeights0;
						QuadFloat weights1 = rowWeights1;
						QuadFloat weights2 = rowWeights2;

						for (size_t x = boundingBox.getMinX(); x <= boundingBox.getMaxX(); x += 4, colorPointer += bufferStepX, depthPointer += bufferStepX)
						{
							const QuadMask positiveWeightsMask = ~(weights0 | weights1 | weights2).castToMask();
							const QuadMask negativeWeightsMask =  (weights0 & weights1 & weights2).castToMask();

							QuadMask       renderMask          = positiveWeightsMask | negativeWeightsMask;

							if (renderMask.moveMask())
							{
								const QuadFloat       normalizedWeights0 = (weights0 / triangle.quadArea).abs();
								const QuadFloat       normalizedWeights1 = (weights1 / triangle.quadArea).abs();
								const QuadFloat       normalizedWeights2 = (weights2 / triangle.quadArea).abs();

								QuadTransformedVertex attributes         = triangle.quadVertex0 * normalizedWeights0 + triangle.quadVertex1 * normalizedWeights1 + triangle.quadVertex2 * normalizedWeights2;

								if (m_depthTest)
								{
									renderMask &= QuadFloat(depthPointer, renderMask).greaterThan(attributes.projectedPosition.z + m_depthBias);
								}

								attributes.textureCoord /= attributes.inverseW;

								shader.draw(renderMask, attributes.projectedPosition, attributes.worldPosition, attributes.normal, attributes.textureCoord, colorPointer, depthPointer);
							}

							weights0 += triangle.quadA12;
							weights1 += triangle.quadA20;
							weights2 += triangle.quadA01;
						}

						rowWeights0 += triangle.quadB12;
						rowWeights1 += triangle.quadB20;
						rowWeights2 += triangle.quadB01;
					}
				}
			}
		}

		void draw(const size_t numThreads, ColorBuffer& colorBuffer, DepthBuffer& depthBuffer) const
		{
			assert(depthBuffer.getWidth()  == colorBuffer.getWidth());
			assert(depthBuffer.getHeight() == colorBuffer.getHeight());
			
			std::vector<std::thread> threads;
			std::atomic<size_t>      tileIndex  = 0;
			
			for (size_t i = 0; i < numThreads; ++i)
			{
				threads.push_back(std::thread(&TileManager::tileThread, this, std::ref(tileIndex), std::ref(colorBuffer), std::ref(depthBuffer)));
			}

			for (std::thread& thread : threads)
			{
				thread.join();
			}
		}

	private:
		static tr::QuadFloat orientPoints(const QuadVec3& lineStarts, const QuadVec3& lineEnds, const QuadVec3& points)
		{
			return (lineEnds.x - lineStarts.x) * (points.y - lineStarts.y) - (lineEnds.y - lineStarts.y) * (points.x - lineStarts.x);
		}

	private:
		std::vector<Tile>     m_tiles;
		std::vector<TShader>  m_shaders;
		std::vector<Triangle> m_triangles;
		bool                  m_depthTest;
		QuadFloat             m_depthBias;
	};
}