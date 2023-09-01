#pragma once

#include <vector>
#include <cassert>
#include <thread>
#include <atomic>
#include <memory>
#include <condition_variable>
#include <string>
#include "trTile.hpp"
#include "trColorBuffer.hpp"
#include "trDepthBuffer.hpp"
#include "trInvalidSettingException.hpp"
#include "trRasterizationParams.hpp"
#include "trTriangle.hpp"
#include "trRenderThread.hpp"

namespace tr
{
	template<typename TShader>
	class TileManager
	{
	public:
		TileManager(const size_t viewportWidth, const size_t viewportHeight, const size_t tileWidth, const size_t tileHeight) :
			m_viewportWidth(0),
			m_viewportHeight(0)
		{
			setAttributes(viewportWidth, viewportHeight, tileWidth, tileHeight);
		}

		void setAttributes(const size_t viewportWidth, const size_t viewportHeight, const size_t tileWidth, const size_t tileHeight)
		{
			if (viewportWidth == 0 || viewportHeight == 0)
			{
				throw InvalidSettingException("Viewport dimensions must be non-zero");
			}
			
			if (tileWidth % 4 != 0)
			{
				throw InvalidSettingException("Tile width must be divisible by 4");
			}

			m_viewportWidth  = viewportWidth;
			m_viewportHeight = viewportHeight;

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

		size_t storeShader(const TShader& shader)
		{
			m_shaders.push_back(shader);

			return m_shaders.size() - 1;
		}

		size_t storeRasterizationParams(const bool depthTest, const float depthBias, const TextureMode textureMode)
		{
			m_rasterizationParams.push_back({ depthTest, depthBias, textureMode });

			return m_rasterizationParams.size() - 1;
		}

		void queue(const Triangle& triangle)
		{
			m_triangles.push_back(triangle);
		}

		void clear()
		{
			m_triangles.clear();
			m_shaders.clear();
			m_rasterizationParams.clear();
		}

		void draw(const size_t numThreads, ColorBuffer& colorBuffer, DepthBuffer& depthBuffer)
		{
			if (size_t(colorBuffer.getWidth()) != m_viewportWidth || size_t(colorBuffer.getHeight()) != m_viewportHeight)
			{
				throw InvalidSettingException(std::string("Color buffer dimensions (") +
				                                          std::to_string(colorBuffer.getWidth()) +
				                                          "," +
				                                          std::to_string(colorBuffer.getHeight()) +
				                                          ") do not match TileManager settings (" +
				                                          std::to_string(m_viewportWidth) +
				                                          "," +
				                                          std::to_string(m_viewportHeight) +
				                                          ")");
			}

			if (size_t(depthBuffer.getWidth()) != m_viewportWidth || size_t(depthBuffer.getHeight()) != m_viewportHeight)
			{
				throw InvalidSettingException(std::string("Depth buffer dimensions (") +
				                                          std::to_string(depthBuffer.getWidth()) +
				                                          "," +
				                                          std::to_string(depthBuffer.getHeight()) +
				                                          ") do not match TileManager settings (" +
				                                          std::to_string(m_viewportWidth) +
				                                          "," +
				                                          std::to_string(m_viewportHeight) +
				                                          ")");
			}

			if (numThreads != m_threads.size())
			{
				initThreads(numThreads);
			}

			std::atomic<size_t> nextTileIndex  = 0;

			for (auto& thread : m_threads)
			{
				thread->draw(nextTileIndex, colorBuffer, depthBuffer);
			}

			for (auto& thread : m_threads)
			{
				thread->wait();
			}
		}

	private:
		void initThreads(const size_t numThreads)
		{
			m_threads.clear();

			for (size_t i = 0; i < numThreads; ++i)
			{
				m_threads.emplace_back(new RenderThread<TShader>(m_tiles, m_triangles, m_shaders, m_rasterizationParams));
			}
		}

	private:
		size_t                                              m_viewportWidth;
		size_t                                              m_viewportHeight;
		std::vector<std::unique_ptr<RenderThread<TShader>>> m_threads;
		std::vector<Tile>                                   m_tiles;
		std::vector<TShader>                                m_shaders;
		std::vector<Triangle>                               m_triangles;
		std::vector<RasterizationParams>                    m_rasterizationParams;
	};
}