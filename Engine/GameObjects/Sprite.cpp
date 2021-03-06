#include "Sprite.h"


namespace S2DE::GameObjects
{
	Sprite::Sprite() :
		m_texture(nullptr),
		m_shader(nullptr),
		m_index_buffer(new Render::IndexBuffer<std::int32_t>()),
		m_vertex_buffer(new Render::VertexBuffer<Render::Vertex>()),
		m_tile_size(DirectX::SimpleMath::Vector2::Zero),
		m_tile_frame_pos(DirectX::SimpleMath::Rectangle(0, 0, 0, 0)),
		m_color(Math::Color<float>(1.0f, 1.0f, 1.0f, 1.0f))
	{
		CreateVertexBuffer();
		CreateIndexBuffer();
		SetDefaultTexture();
		SetDefaultShader();
	}

	Sprite::~Sprite()
	{
		//TODO: Need to check count of using
		if (m_unload_texture == true)
			Core::Engine::GetResourceManager().Erase<Render::Texture>(m_texture->GetName());

		Core::Delete(m_vertex_buffer);
		Core::Delete(m_index_buffer);
		Core::Delete(m_shader);
		Core::Delete(m_sprite_const_buf);
		Core::Delete(m_texture);
	}

	void Sprite::SetColor(Math::Color<float> color)
	{	
		m_color = color;
		//Update vertex buffer
		CreateVertexBuffer();
	}

	void Sprite::SetAtlasFramePosition(std::int32_t x, std::int32_t y)
	{
		m_tile_frame_pos = DirectX::SimpleMath::Rectangle(x, y, 0, 0);
	}

	void Sprite::SetAtlasSize(DirectX::SimpleMath::Vector2 size)
	{
		m_tile_size = size;
	}

	bool Sprite::LoadTexture(std::string name, bool unload_texture, bool auto_load_texture)
	{
		//Set unload state
		m_unload_texture = unload_texture;

		//If texture not found in resource manager storage we try to load it 
		if (!Core::Engine::GetResourceManager().IsExists<Render::Texture>(name)
			&& auto_load_texture == true)
		{
			if (!Core::Engine::GetResourceManager().Load<Render::Texture>(name))
				return false;
		}
		//Set texture if texture is exist
		m_texture = new Render::Texture(*Core::Engine::GetResourceManager().Get<Render::Texture>(name));
		S2DE_ASSERT(m_texture != nullptr);	
		return true;
	}

	void Sprite::OnRender()
	{
		//Bind and update variables in const buffer
		m_shader->UpdateMainConstBuffer(UpdateTransformation());

		m_sprite_const_buf->Lock();
		m_sprite_const_buf->GetData()->sprite_tile_frame	= DirectX::XMINT2(m_tile_frame_pos.x, m_tile_frame_pos.y);
		m_sprite_const_buf->GetData()->sprite_tile_size		= DirectX::SimpleMath::Vector2(m_tile_size.x, m_tile_size.y);
		m_sprite_const_buf->GetData()->sprite_texture_res	= DirectX::SimpleMath::Vector2((float)m_texture->GetWidth(), (float)m_texture->GetHeight());
		m_sprite_const_buf->Unlock();
		m_sprite_const_buf->Bind(1);

		//Bind shader and texture 
		m_shader->Bind();
		m_texture->Bind();

		//Bind buffers
		m_vertex_buffer->Bind();
		m_index_buffer->Bind();

		//Draw poly 
		Core::Engine::GetRenderer()->GetContext()->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
		Core::Engine::GetRenderer()->GetContext()->DrawIndexed(m_index_buffer->GetArray().size(), 0, 0);

		//Unbind 
		m_shader->Unbind();
		m_texture->Unbind();
		m_vertex_buffer->Unbind();
		m_index_buffer->Unbind();
		m_sprite_const_buf->Unbind();
	}

	DirectX::SimpleMath::Matrix Sprite::UpdateTransformation()
	{
		m_WorldMatrix = DirectX::XMMatrixTransformation(
			//Scale
			//Center | Rotation | Scaling
			DirectX::SimpleMath::Vector3::Zero, DirectX::SimpleMath::Vector3::Zero, m_Scale * CalcScaleFactor(),
			//Rotation
			//Center | Quatarnion
			DirectX::SimpleMath::Vector3::Zero, ToQuaternion(m_Rotation),
			//Translation
			m_Position);

		m_WorldMatrix.Transpose(m_WorldMatrix);

		return m_WorldMatrix;
	}

	void Sprite::CreateVertexBuffer()
	{	 
		m_vertex_buffer->GetArray() =
		{
			{ DirectX::SimpleMath::Vector3(-1.0f,   -1.0f,   0.0f),  DirectX::SimpleMath::Vector4(m_color.r, m_color.g, m_color.b, m_color.a),  DirectX::SimpleMath::Vector2(0.0f, 1.0f) }, // Bottom left.
			{ DirectX::SimpleMath::Vector3(-1.0f,   1.0f,   0.0f),   DirectX::SimpleMath::Vector4(m_color.r, m_color.g, m_color.b, m_color.a),  DirectX::SimpleMath::Vector2(0.0f, 0.0f) }, // Top left.
			{ DirectX::SimpleMath::Vector3(1.0f,  1.0f,   0.0f),	 DirectX::SimpleMath::Vector4(m_color.r, m_color.g, m_color.b, m_color.a),  DirectX::SimpleMath::Vector2(1.0f, 0.0f) }, // top right.
			{ DirectX::SimpleMath::Vector3(1.0f,  -1.0f,   0.0f),    DirectX::SimpleMath::Vector4(m_color.r, m_color.g, m_color.b, m_color.a),  DirectX::SimpleMath::Vector2(1.0f, 1.0f) }, // Bottom right.
		};

		S2DE_ASSERT(m_vertex_buffer->Create());
		m_vertex_buffer->Update();
	}	 
		 
	void Sprite::CreateIndexBuffer()
	{	 
		m_index_buffer->GetArray() =
		{
				0, 1, 2,
				0, 2, 3,
		};

		S2DE_ASSERT(m_index_buffer->Create());
		m_index_buffer->Update();
	}	 
		 
	void Sprite::UpdateTexture()
	{
		//Get texture name
		std::string name = m_texture->GetName();
		//Delete previous texture
		Core::Delete(m_texture);

		//Try to get texture by name from resource manager
		auto new_texture = Core::Engine::GetResourceManager().Get<Render::Texture>(name);

		//If texture not found
		if (new_texture == Core::Engine::GetResourceManager().GetDefaultTexture())
		{
			Logger::Error("%s Can't update texture!", GetName().c_str());
			return;
		}

		m_texture = new Render::Texture(*new_texture);
		S2DE_ASSERT(m_texture != nullptr);
	}
	 
	void Sprite::UpdateShader()
	{
		//Get shader name
		std::string name = m_shader->GetName();
		//Delete previous shader 
		Core::Delete(m_shader);

		//Try to get shader by name from resource manager
		auto new_shader = Core::Engine::GetResourceManager().Get<Render::Shader>(name);

		//If shader not found
		if (new_shader == nullptr)
		{
			Logger::Error("%s Can't update shader!", GetName().c_str());
			return;
		}

		m_shader = new Render::Shader(*new_shader);
		S2DE_ASSERT(m_shader != nullptr);
		
		//Create constant buffer with sprite const buffer type
		m_sprite_const_buf = new Render::ConstantBuffer<SpriteConstBuffer>();
		S2DE_ASSERT(m_sprite_const_buf->Create());
	}

	void Sprite::SetDefaultShader()
	{	 
		m_shader = new Render::Shader(*Core::Engine::GetResourceManager().Get<Render::Shader>("Sprite"));
		m_sprite_const_buf = new Render::ConstantBuffer<SpriteConstBuffer>();
		S2DE_ASSERT(m_sprite_const_buf->Create());
	}	 
		 
	inline DirectX::SimpleMath::Vector3 Sprite::CalcScaleFactor()
	{
		return m_tile_size == DirectX::SimpleMath::Vector2::Zero ? DirectX::SimpleMath::Vector3(m_texture->GetWidth() * 0.01f, m_texture->GetHeight() * 0.01f, 1.0f) :
			DirectX::SimpleMath::Vector3(m_tile_size.x * 0.01f, m_tile_size.y * 0.01f, 1.0f);
	}

	void Sprite::SetDefaultTexture()
	{
		m_texture = new Render::Texture(*Core::Engine::GetResourceManager().GetDefaultTexture());
	}
}