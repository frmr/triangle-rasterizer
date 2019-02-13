#include "trTexture.hpp"

#include "../lodepng/lodepng.h"

tr::Texture::Texture(const size_t width, const size_t height)
{
	init(width, height);
}

tr::Texture::Texture(const std::string& filename)
{
	std::vector<uint8_t> encodedData;

	if (!lodepng::load_file(encodedData, filename))
	{
		uint32_t             width;
		uint32_t             height;
		lodepng::State       state;
		std::vector<uint8_t> decodedData;

		state.info_raw.colortype = LodePNGColorType::LCT_RGBA;

		if (!lodepng::decode(decodedData, width, height, state, encodedData))
		{
			init(width, height);
			copyImageDataToBaseLevel(decodedData);
		}
	}
}

tr::Error tr::Texture::generateMipmaps()
{
	if (m_mipLevels.size() == 1)
	{
		if (!isPowerOfTwo(m_baseLevel->getWidth()) || !isPowerOfTwo(m_baseLevel->getHeight()))
		{
			return Error::InvalidBufferSize;
		}

		m_mipLevels.reserve(std::max(size_t(std::log2(m_baseLevel->getWidth())), size_t(std::log2(m_baseLevel->getHeight()))));
		m_baseLevel = m_mipLevels.data();
	}

	ColorBuffer* source = m_mipLevels.data();

	do
	{
		m_mipLevels.emplace_back(source->getHeight() / 2, source->getHeight() / 2);

		for (size_t sourceY = 0, destY = 0; sourceY < source->getHeight(); sourceY += 2, ++destY)
		{
			for (size_t sourceX = 0, destX = 0; sourceX < source->getWidth(); sourceX += 2, ++destX)
			{
				Color& tl = source->at(sourceX,     sourceY    );
				Color& tr = source->at(sourceX + 1, sourceY    );
				Color& bl = source->at(sourceX,     sourceY + 1);
				Color& br = source->at(sourceX + 1, sourceY + 1);

				m_mipLevels.back().at(destX, destY) = Color(uint8_t((uint16_t(tl.b) + uint16_t(tr.b) + uint16_t(bl.b) + uint16_t(br.b)) / 4),
				                                            uint8_t((uint16_t(tl.g) + uint16_t(tr.g) + uint16_t(bl.g) + uint16_t(br.g)) / 4),
				                                            uint8_t((uint16_t(tl.r) + uint16_t(tr.r) + uint16_t(bl.r) + uint16_t(br.r)) / 4),
				                                            uint8_t((uint16_t(tl.a) + uint16_t(tr.a) + uint16_t(bl.a) + uint16_t(br.a)) / 4));
			}
		}

		source = &m_mipLevels.back();
	}
	while (source->getWidth() > 1 && source->getHeight() > 1);
}

size_t tr::Texture::getWidth() const
{
	return m_baseLevel->getWidth();
}

size_t tr::Texture::getHeight() const
{
	return m_baseLevel->getHeight();
}

tr::ColorBuffer& tr::Texture::getMipLevel(const size_t mipLevel)
{
	return m_mipLevels[mipLevel];
}

tr::Color tr::Texture::getAt(float u, float v, const bool filter, const TextureWrappingMode textureWrappingMode) const
{
	return m_baseLevel->getAt(u, v, filter, textureWrappingMode);
}

void tr::Texture::init(const size_t width, const size_t height)
{
	m_mipLevels.push_back(ColorBuffer(width, height));
	m_baseLevel = m_mipLevels.data();
}

void tr::Texture::copyImageDataToBaseLevel(const std::vector<uint8_t>& decodedData)
{
	for (size_t y = 0, i = 0; y < m_baseLevel->getHeight(); ++y)
	{
		for (size_t x = 0; x < m_baseLevel->getWidth(); ++x, i += 4)
		{
			const uint8_t* const pixelData = &decodedData[i];

			m_baseLevel->at(x, y) = Color(
				*(pixelData + 2),
				*(pixelData + 1),
				*(pixelData + 0),
				*(pixelData + 3)
			);
		}
	}
}

bool tr::Texture::isPowerOfTwo(const size_t x)
{
	return (x != 0) && ((x & (x - 1)) == 0);
}
