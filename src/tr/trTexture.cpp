#include "trTexture.hpp"

tr::Texture::Texture(const size_t width, const size_t height)
{
	init(width, height);
}

tr::Texture::Texture(const size_t width, const size_t height, const std::vector<uint8_t>& rgbaData) :
	m_maxMipLevelIndex(0),
	m_baseLevel(nullptr)
{
	init(width, height);
	copyImageDataToBaseLevel(rgbaData);
}

bool tr::Texture::isInitialized() const
{
	return !m_mipLevels.empty();
}

tr::Error tr::Texture::generateMipmaps()
{
	if (m_mipLevels.size() == 1)
	{
		if (!isPowerOfTwo(m_baseLevel->getWidth()) || !isPowerOfTwo(m_baseLevel->getHeight()))
		{
			return Error::InvalidBufferSize;
		}

		m_mipLevels.reserve(std::max(size_t(std::log2(m_baseLevel->getWidth())), size_t(std::log2(m_baseLevel->getHeight()))) + 1);
		m_baseLevel = m_mipLevels.data();
	}

	ColorBuffer* source = m_mipLevels.data();

	do
	{
		m_mipLevels.emplace_back(source->getWidth() / 2, source->getHeight() / 2);

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

	m_maxMipLevelIndex = m_mipLevels.size() - 1;

	return Error::Success;
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

const tr::ColorBuffer& tr::Texture::getConstMipLevel(const size_t mipLevel) const
{
	return m_mipLevels[mipLevel];
}

size_t tr::Texture::getNumMipLevels() const
{
	return m_mipLevels.size();
}

tr::Color tr::Texture::getAt(const float u, const float v, const bool filter, const TextureWrappingMode textureWrappingMode) const
{
	return m_baseLevel->getAt(u, v, filter, textureWrappingMode);
}

tr::Color tr::Texture::getAt(const float u, const float v, const bool filter, const TextureWrappingMode textureWrappingMode, const float du, const float dv, const bool interpolateMipmapLevels) const
{
	const float dx       = du * m_baseLevel->getFloatWidth();
	const float dy       = dv * m_baseLevel->getFloatHeight();
	const float mipLevel = fastLog2(std::max(dx, dy));

	if (interpolateMipmapLevels)
	{
		const size_t  floor      = std::min(size_t(mipLevel), m_maxMipLevelIndex);
		const size_t  ceil       = std::min(floor + 1,        m_maxMipLevelIndex);

		const float   ceilRatio  = mipLevel - float(floor);
		const float   floorRatio = 1.0f - ceilRatio;

		const Vector4 floorColor = m_mipLevels[floor].getAt(u, v, filter, textureWrappingMode).toVector();
		const Vector4 ceilColor  = m_mipLevels[ceil].getAt(u, v, filter, textureWrappingMode).toVector();

		return Color(floorColor * floorRatio + ceilColor * ceilRatio);
	}
	else
	{
		return m_mipLevels[std::min(size_t(std::lroundf(mipLevel)), m_mipLevels.size() - 1)].getAt(u, v, filter, textureWrappingMode);
	}
}

void tr::Texture::init(const size_t width, const size_t height)
{
	m_mipLevels.push_back(ColorBuffer(width, height));

	m_maxMipLevelIndex = m_mipLevels.size() - 1;
	m_baseLevel        = m_mipLevels.data();
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

float tr::Texture::fastLog2(const float x)
{
	union { float f; uint32_t i; } vx = { x };

	return vx.i * 1.1920928955078125e-7f - 126.94269504f;
}
