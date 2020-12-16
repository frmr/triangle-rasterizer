#pragma once

#include <condition_variable>
#include <thread>
#include "trColorBuffer.hpp"
#include "trDepthBuffer.hpp"
#include "trTile.hpp"
#include "trTriangle.hpp"
#include "trRasterizationParams.hpp"

namespace tr
{
	template <typename TShader>
	class RenderThread
	{
	public:
		RenderThread(const std::vector<Tile>& tiles, const std::vector<Triangle>& triangles, const std::vector<TShader>& shaders, const std::vector<RasterizationParams>& rasterizationParams) :
			m_quit(false),
			m_draw(false),
			m_tiles(tiles),
			m_triangles(triangles),
			m_shaders(shaders),
			m_rasterizationParams(rasterizationParams),
			m_nextTileIndex(nullptr),
			m_colorBuffer(nullptr),
			m_depthBuffer(nullptr),
			m_continueConditionVariable(),
			m_waitConditionVariable(),
			m_mutex(),
			m_thread(&RenderThread::threadFunction, this)
		{
		}
		
		~RenderThread()
		{
			kill();
		}

		void draw(std::atomic<size_t>& nextTileIndex, tr::ColorBuffer& colorBuffer, tr::DepthBuffer& depthBuffer)
		{
			m_nextTileIndex = &nextTileIndex;
			m_colorBuffer   = &colorBuffer;
			m_depthBuffer   = &depthBuffer;

			m_draw = true;

			m_continueConditionVariable.notify_one();
		}

		void wait()
		{
			std::unique_lock<std::mutex> lock(m_mutex);
			m_waitConditionVariable.wait(lock, [&]{ return !m_draw; });
		}

		void kill()
		{
			m_quit = true;
			m_continueConditionVariable.notify_one();

			if (m_thread.joinable())
			{
				m_thread.join();
			}
		}

	private:
		void threadFunction()
		{
			while (!m_quit)
			{
				std::unique_lock<std::mutex> lock(m_mutex);
				m_continueConditionVariable.wait(lock, [&]{ return m_draw || m_quit; });

				if (m_draw)
				{
					doStuff();

					m_draw = false;

					lock.unlock();
					m_waitConditionVariable.notify_one();
				}
			}
		}

		void doStuff() const
		{
			size_t myTileIndex;

			while ((myTileIndex  = m_nextTileIndex->fetch_add(1, std::memory_order_relaxed)) < m_tiles.size())
			{
				const Tile& tile = m_tiles[myTileIndex];

				for (const Triangle& triangle : m_triangles)
				{
					const Rect boundingBox = triangle.boundingBox.intersection(tile.getBounds());

					if (!boundingBox.isValid())
					{
						continue;
					}

					const TShader&             shader              = m_shaders[triangle.shaderIndex];
					const RasterizationParams& rasterizationParams = m_rasterizationParams[triangle.rasterizationParamsIndex];

					const QuadFloat             quadA01(4.0f * (triangle.vertices[0].projectedPosition.y - triangle.vertices[1].projectedPosition.y));
					const QuadFloat             quadB01(1.0f * (triangle.vertices[1].projectedPosition.x - triangle.vertices[0].projectedPosition.x));
					const QuadFloat             quadA12(4.0f * (triangle.vertices[1].projectedPosition.y - triangle.vertices[2].projectedPosition.y));
					const QuadFloat             quadB12(1.0f * (triangle.vertices[2].projectedPosition.x - triangle.vertices[1].projectedPosition.x));
					const QuadFloat             quadA20(4.0f * (triangle.vertices[2].projectedPosition.y - triangle.vertices[0].projectedPosition.y));
					const QuadFloat             quadB20(1.0f * (triangle.vertices[0].projectedPosition.x - triangle.vertices[2].projectedPosition.x));
					const QuadTransformedVertex quadVertex0(triangle.vertices[0]);
					const QuadTransformedVertex quadVertex1(triangle.vertices[1]);
					const QuadTransformedVertex quadVertex2(triangle.vertices[2]);
					const QuadFloat             quadArea(orientPoints(quadVertex0.projectedPosition, quadVertex1.projectedPosition, quadVertex2.projectedPosition));

					const size_t   bufferStepX  = 4;
					const size_t   bufferStepY  = m_depthBuffer->getWidth() - (boundingBox.getMaxX() - boundingBox.getMinX()) + (boundingBox.getMaxX() - boundingBox.getMinX()) % bufferStepX - bufferStepX;

					Color*         colorPointer = m_colorBuffer->getData() + boundingBox.getMinY() * m_colorBuffer->getWidth() + boundingBox.getMinX();
					float*         depthPointer = m_depthBuffer->getData() + boundingBox.getMinY() * m_depthBuffer->getWidth() + boundingBox.getMinX();

					const QuadVec3 points(
						QuadFloat(float(boundingBox.getMinX()), float(boundingBox.getMinX() + 1), float(boundingBox.getMinX() + 2), float(boundingBox.getMinX() + 3)),
						QuadFloat(float(boundingBox.getMinY()), float(boundingBox.getMinY()    ), float(boundingBox.getMinY()    ), float(boundingBox.getMinY()    )),
						0.0f
					);
					
					QuadFloat rowWeights0 = orientPoints(quadVertex1.projectedPosition, quadVertex2.projectedPosition, points);
					QuadFloat rowWeights1 = orientPoints(quadVertex2.projectedPosition, quadVertex0.projectedPosition, points);
					QuadFloat rowWeights2 = orientPoints(quadVertex0.projectedPosition, quadVertex1.projectedPosition, points);

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
								const QuadFloat       normalizedWeights0 = (weights0 / quadArea).abs();
								const QuadFloat       normalizedWeights1 = (weights1 / quadArea).abs();
								const QuadFloat       normalizedWeights2 = (weights2 / quadArea).abs();

								QuadTransformedVertex attributes         = quadVertex0 * normalizedWeights0 + quadVertex1 * normalizedWeights1 + quadVertex2 * normalizedWeights2;

								if (rasterizationParams.depthTest)
								{
									renderMask &= QuadFloat(depthPointer, renderMask).greaterThan(attributes.projectedPosition.z + rasterizationParams.depthBias);
								}

								if (rasterizationParams.textureMode == TextureMode::Perspective)
								{
									attributes.textureCoord /= attributes.inverseW;
								}

								shader.draw(renderMask, attributes.projectedPosition, attributes.worldPosition, attributes.normal, attributes.textureCoord, colorPointer, depthPointer);
							}

							weights0 += quadA12;
							weights1 += quadA20;
							weights2 += quadA01;
						}

						rowWeights0 += quadB12;
						rowWeights1 += quadB20;
						rowWeights2 += quadB01;
					}
				}
			}
		}

		static tr::QuadFloat orientPoints(const QuadVec3& lineStarts, const QuadVec3& lineEnds, const QuadVec3& points)
		{
			return (lineEnds.x - lineStarts.x) * (points.y - lineStarts.y) - (lineEnds.y - lineStarts.y) * (points.x - lineStarts.x);
		}

	private:
		bool                                    m_quit;
		bool                                    m_draw;
		const std::vector<Tile>&                m_tiles;
		const std::vector<Triangle>&            m_triangles;
		const std::vector<TShader>&             m_shaders;
		const std::vector<RasterizationParams>& m_rasterizationParams;
		std::atomic<size_t>*                    m_nextTileIndex;
		
		ColorBuffer*                            m_colorBuffer;
		DepthBuffer*                            m_depthBuffer;
		std::condition_variable                 m_continueConditionVariable;
		std::condition_variable                 m_waitConditionVariable;
		std::mutex                              m_mutex;
		std::thread                             m_thread;
	};
}
