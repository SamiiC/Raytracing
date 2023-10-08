#include "Renderer.h"
#include "Walnut/Random.h"
 

namespace Utils
{
	static uint32_t ConvertToRGBA(const glm::vec4& colour)
	{
		uint8_t r = (colour.r * 255.0f);
		uint8_t g = (colour.g * 255.0f);
		uint8_t b = (colour.b * 255.0f);
		uint8_t a = (colour.a * 255.0f);


		// put in each colour channel into its 8 bit place
		uint32_t res = (a << 24) | (b << 16) | (g << 8) | r;
		return res;

	}
}



void Renderer::OnResize(uint32_t width, uint32_t height)
{

	if (m_FinalImage)
	{
		// image exists, but hasnt been resized so no need to resize
		if (m_FinalImage->GetWidth() == width && m_FinalImage->GetHeight() == height)
			return;

		m_FinalImage->Resize(width, height);
	}
	else 
	{
		//image doesnt exist so create
		m_FinalImage = std::make_shared<Walnut::Image>(width, height, Walnut::ImageFormat::RGBA);
	}

	delete[] m_ImageData;
	m_ImageData = new uint32_t[width * height];


}

void Renderer::Render()
{
	// render each pixel

	for (uint32_t y = 0; y < m_FinalImage->GetHeight(); y++)
	{
		for (uint32_t x = 0; x < m_FinalImage->GetWidth(); x++)
		{

			glm::vec2 coord = { x / (float)m_FinalImage->GetWidth() , y / (float)m_FinalImage->GetHeight() };
			coord = coord * 2.0f - 1.0f; // -1 -> 1 mapping

			glm::vec4 colour = PerPixel(coord);
			colour = glm::clamp(colour, glm::vec4(0.0f), glm::vec4(1.0f));

			m_ImageData[
				(x + y * m_FinalImage->GetWidth())
			] = Utils::ConvertToRGBA(colour);

		}
	} 


	m_FinalImage->SetData(m_ImageData);

}

glm::vec4 Renderer::PerPixel(glm::vec2 coord)
{
	float RADIUS = 0.5f;

	// Define rays
	glm::vec3 rayOrigin(0.0f,0.0f,1.0f);

	glm::vec3 rayDirection(coord.x, coord.y, -1.0f);


	// Calculations to get scalar and find hit positions of rays on sphere (simple quadratic solving)
	float a = glm::dot(rayDirection, rayDirection);
	float b = 2.0f * glm::dot(rayOrigin, rayDirection);
	float c = glm::dot(rayOrigin, rayOrigin) - RADIUS * RADIUS;


	float discrim = b * b - 4.0f * a * c;

	if (discrim < 0.0f)
		return glm::vec4(0, 0, 0, 1);


	float t1 = (-b + glm::sqrt(discrim)) / (2.0f * a);
	float t2 = (-b - glm::sqrt(discrim)) / (2.0f * a);

	glm::vec3 h1 = rayOrigin + t1 * rayDirection;
	glm::vec3 h2 = rayOrigin + t2 * rayDirection;

	// normalising hit vector correctly gives colour
	glm::vec3 normal = glm::normalize(h2);



	
	glm::vec3 lightDir = glm::normalize(glm::vec3(-1, -1, -1));


	//calc angle between light and normal
	// we already have unit vectors so angle = arccos(dot product)
	float intensity  = glm::max(glm::dot(normal, -lightDir), 0.0f);
	


	glm::vec3 sphereColour(1, 0, 1);
	sphereColour *= intensity;
	return glm::vec4(sphereColour, 1); 



}