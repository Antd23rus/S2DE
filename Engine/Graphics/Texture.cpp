#include "Texture.h"
#include "Base/Engine.h"
#include "Graphics/Renderer.h"

#include <WICTextureLoader.h>
#include <DDSTextureLoader.h>

namespace S2DE::Render
{
	Texture::Texture() 
	{
		m_type = "Texture";
		m_ex = 
		{ 
			// DDS loader supports
			".dds", 
			// WIC loader supports 
			".bmp", 
			".tiff", 
			".gif",  
			".png", 
			".tga", 
			".jpg" 
		};
	}

	Texture::~Texture()
	{

	}

	void Texture::Cleanup()
	{
		Core::Release(m_resource);
		Core::Release(m_resourceview);
		Core::Release(m_texture2d);
		Core::Release(m_texture_sampler_state);
	}

	void Texture::UpdateTextureDesc()
	{
		m_resourceview->GetResource(&m_resource);
		m_resource->QueryInterface<ID3D11Texture2D>(&m_texture2d);
		m_texture2d->GetDesc(&m_texture_desc);
	}

	bool Texture::CreateSamplerState()
	{
		D3D11_SAMPLER_DESC sampler_desc = { };
		sampler_desc.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
		sampler_desc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
		sampler_desc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
		sampler_desc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
		sampler_desc.ComparisonFunc = D3D11_COMPARISON_NEVER;
		sampler_desc.MinLOD = 0;
		sampler_desc.MaxLOD = D3D11_FLOAT32_MAX;
		sampler_desc.MipLODBias = 0.0f;
		sampler_desc.MaxAnisotropy = 1;
		sampler_desc.BorderColor[0] = 0;
		sampler_desc.BorderColor[1] = 0;
		sampler_desc.BorderColor[2] = 0;
		sampler_desc.BorderColor[3] = 0;

		S2DE_CHECK(Core::Engine::GetRenderer()->GetDevice()->CreateSamplerState(&sampler_desc, &m_texture_sampler_state), "Can't create sampler state");
		return true;
	}

	bool Texture::Load(std::string path)
	{
		//If path is empty 
		if (Core::Other::isStringEmpty(path))
		{
			Logger::Error("Path string is empty, can't load texture!");
			return false;
		}

		HRESULT hr = S_OK;

		//Get file extension because for dds format we need to use special function
		std::string fileExtension = Core::Other::GetFileExtension(path);
		if (fileExtension == ".dds")
		{
			hr = DirectX::CreateDDSTextureFromFile(Core::Engine::GetRenderer()->GetDevice(), Core::Other::StringToWString(path).c_str(), &m_resource, &m_resourceview);
			if (FAILED(hr))
			{
				Logger::Error("Can't create dds texture from file Path:%s Details:%s Extension:%s", path.c_str(), Core::Utils::GetHRCodeDetails(hr).c_str(), fileExtension.c_str());
				return false;
			}
		}
		else
		{
			hr = DirectX::CreateWICTextureFromFile(Core::Engine::GetRenderer()->GetDevice(), Core::Other::StringToWString(path).c_str(), &m_resource, &m_resourceview);
			if (FAILED(hr))
			{
				Logger::Error("Can't create texture from file Path:%s Details:%s Extension:%s", path.c_str(), Core::Utils::GetHRCodeDetails(hr).c_str(), fileExtension.c_str());
				return false;
			}
		}
		
		//Create sampler state for current texture
		CreateSamplerState();

		//Get and save texture description
		UpdateTextureDesc();
		return true;
	}

	bool Texture::CreateEmptyTexture(Math::Color<std::uint32_t> color)
	{
		const uint32_t pixel = color.r | (color.g << 8) | (color.b << 16) | (color.a << 24); 
		D3D11_SUBRESOURCE_DATA initData = { &pixel, sizeof(uint32_t), 0 };
		D3D11_TEXTURE2D_DESC texture_desc = { };

		texture_desc.Width = 1;
		texture_desc.Height = 1;
		texture_desc.MipLevels = 1;
		texture_desc.ArraySize = 1;
		texture_desc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
		texture_desc.SampleDesc.Count = 1;
		texture_desc.Usage	= D3D11_USAGE_IMMUTABLE;
		texture_desc.BindFlags = D3D11_BIND_SHADER_RESOURCE;

		ID3D11Texture2D* texture = nullptr;
		S2DE_CHECK(Core::Engine::GetRenderer()->GetDevice()->CreateTexture2D(&texture_desc, &initData, &texture), "Can't create empty texture");

		D3D11_SHADER_RESOURCE_VIEW_DESC shader_desc = { };

		shader_desc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
		shader_desc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
		shader_desc.Texture2D.MipLevels = 1;

		S2DE_CHECK(Core::Engine::GetRenderer()->GetDevice()->CreateShaderResourceView(texture, &shader_desc, &m_resourceview), "Can't create shader resource for empty texture");

		UpdateTextureDesc();
		return true;
	}

	void Texture::Unbind()
	{
		Core::Engine::GetRenderer()->GetContext()->PSSetSamplers(0, 0, nullptr);
		Core::Engine::GetRenderer()->GetContext()->PSSetShaderResources(0, 0, nullptr);
	}

	void Texture::Bind(std::uint32_t NumViews)
	{
		Core::Engine::GetRenderer()->GetContext()->PSSetShaderResources(0, NumViews, &m_resourceview);
		//Check on sampler exist 
		if(m_texture_sampler_state != nullptr)
			Core::Engine::GetRenderer()->GetContext()->PSSetSamplers(0, 1, &m_texture_sampler_state);
	}
}