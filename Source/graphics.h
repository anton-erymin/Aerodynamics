#pragma once

#define GLEW_STATIC
#include "glew.h"

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

#include <fstream>

#include "objloader.h"

#include "glm\mat4x4.hpp"
#include "glm\gtc\vec1.hpp"



#define  MAX_SHADER_LENGTH		4096


// Вершина внутреннего формата
struct Vert
{
	// Позиция
	GLfloat pos[3];
	// Текстурные координаты
	GLfloat tex[2];
	// Нормаль
	GLfloat nor[3];
};





// Материал
struct Material
{
	// Название
	char name[32];

	// Диффузный цвет
	glm::vec4 diffuse_color;
	// Фоновый цвет
	glm::vec4 ambient_color;
	// Цвет бликов
	glm::vec4 specular_color;
	// Прозрачность
	float alpha;
	// Гладкость
	float shininess;
	
	// Имя файла диффузной текстура
	char map_Kd[128];
	char map_bump[128];

	// Хэндл текстуры
	GLuint diff_texture;



	Material()
	{
		strcpy(map_Kd, "");
		strcpy(map_bump, "");
	}
};


// Структура меша внутреннего формата
struct Mesh
{
	// Размер буфера вершин
	GLsizei vert_len;
	// Размер буфера индексов
	GLsizei inds_len;

	// Данные вершин
	Vert *verts;
	// Индексы
	GLuint *inds;

	// Кол-во подмешей
	GLsizei submeshes_len;
	// Названия подмешей
	char **submeshes_names;
	// Данные подмешей (3 элемента: 1-й - смещение в буфере индексов, 2-й - кол-во индексов, 3-й - индекс материала (или -1 если нет)"
	GLint *submeshes;

	// Хэндлы буферов
	GLuint vert_buf, el_buf;

	// Материалы меша
	Material *materials;
};


// Программа
struct ShaderProgram
{
	// Хэндлы шейдеров и программы
	GLuint vertex_shader, fragment_shader, program;

	// Хэндлы атрибутов
	struct
	{
		GLuint position;
		GLuint normal;
		GLuint texcoord;
	} attribs;

	// Хэндлы юнифоромов
	struct
	{
		GLuint p_mat;
		GLuint mv_mat;
		GLuint cam_mat;
		GLuint shadow_mat;

		GLuint diffuse_color;
		GLuint specular_color;
		GLuint ambient_color;
		GLuint shininess;

		GLuint texture;
		GLuint shadowmap;
	} uniforms;
};



struct ShadowMap
{
	GLuint framebuffer;
	GLuint depthTexture;

	glm::mat4 light_v_mat;
	glm::mat4 light_p_mat;
	glm::mat4 shadow_mat;
};

glm::mat4 normalization_mat(
0.5, 0.0, 0.0, 0.0,
0.0, 0.5, 0.0, 0.0,
0.0, 0.0, 0.5, 0.0,
0.5, 0.5, 0.5, 1.0
);





GLuint make_buffer(GLenum target, const void *data, GLsizei size)
{
	GLuint buffer;
	glGenBuffers(1, &buffer);
	glBindBuffer(target, buffer);
	glBufferData(target, size, data, GL_STATIC_DRAW);

	GLenum res = glGetError();

	if (res == GL_OUT_OF_MEMORY)
	{
		printf("Out of memory\n");
	}

	if (buffer > 0)
	{
		printf("VBO created.\n");
	}

	return buffer;
}


void make_mesh_buffers(Mesh& mesh)
{
	mesh.vert_buf	= make_buffer(GL_ARRAY_BUFFER,			mesh.verts, mesh.vert_len * sizeof(Vert));
	mesh.el_buf		= make_buffer(GL_ELEMENT_ARRAY_BUFFER,  mesh.inds, mesh.inds_len * sizeof(GLuint));
}


void* loadBitmap(const char* FilePath, GLsizei &width, GLsizei &height);

GLuint make_texture(const char *fname)
{
	GLsizei width, height;
	void *data = loadBitmap(fname, width, height);

	GLuint texture;
	glGenTextures(1, &texture);
	glBindTexture(GL_TEXTURE_2D, texture);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB8, width, height, 0, GL_BGR, GL_UNSIGNED_BYTE, data);

	delete[] data;

	return texture;
}



GLuint make_shader(GLenum type, const char *fname)
{
	std::ifstream s;
	s.open(fname);

	
	
	char buf[512];
	char *source = new char[MAX_SHADER_LENGTH];
	source[0] = 0;
	while (!s.eof())
	{
		s.getline(buf, 512);
		strcat(source, buf);
		strcat(source, "\n");
	}
	s.close();
	
	GLuint shader = glCreateShader(type);
	GLint len = strlen(source);
	glShaderSource(shader, 1, (GLchar**)&source, &len);
	glCompileShader(shader);

	delete[] source;

	GLint res;
	glGetShaderiv(shader, GL_COMPILE_STATUS, &res);
	if (!res)
	{
		char infolog[512] = "";
		GLsizei len;
		glGetShaderInfoLog(shader, 512, &len, infolog);
		fprintf(stderr, "Failed to compile %s: %s\n", fname, infolog);
		glDeleteShader(shader);
		return 0;
	}

	return shader;
}


bool make_program(const char *fname_v, const char *fname_f, ShaderProgram& prog)
{
	prog.program = glCreateProgram();

	if (strcmp(fname_v, "") != 0)
	{
		prog.vertex_shader	 = make_shader(GL_VERTEX_SHADER, fname_v);
		if (prog.vertex_shader == 0)
			return 0;

		glAttachShader(prog.program, prog.vertex_shader);
	}


	if (strcmp(fname_f, "") != 0)
	{
		prog.fragment_shader = make_shader(GL_FRAGMENT_SHADER, fname_f);
		if (prog.fragment_shader == 0)
			return 0;

		glAttachShader(prog.program, prog.fragment_shader);
	}
	
	
	glLinkProgram(prog.program);

	GLint res;
	glGetProgramiv(prog.program, GL_LINK_STATUS, &res);
	if (!res)
	{
		char infolog[512] = "";
		GLsizei len;
		glGetProgramInfoLog(prog.program, 512, &len, infolog);
		fprintf(stderr, "Failed to link program: %s\n", infolog);
		glDeleteProgram(prog.program);
		return false;
	}

	return true;
}


void render_mesh(const Mesh& mesh, const ShaderProgram& shader)
{
	glBindBuffer(GL_ARRAY_BUFFER, mesh.vert_buf);

	glVertexAttribPointer(shader.attribs.position, 3, GL_FLOAT, GL_FALSE, sizeof(Vert), 0);
	glVertexAttribPointer(shader.attribs.normal, 3, GL_FLOAT, GL_FALSE, sizeof(Vert),   (void*)offsetof(struct Vert, nor));
	glVertexAttribPointer(shader.attribs.texcoord, 2, GL_FLOAT, GL_FALSE, sizeof(Vert), (void*)offsetof(struct Vert, tex));

	glEnableVertexAttribArray(shader.attribs.position);
	glEnableVertexAttribArray(shader.attribs.normal);
	glEnableVertexAttribArray(shader.attribs.texcoord);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mesh.el_buf);
	glDrawElements(GL_TRIANGLES, mesh.inds_len, GL_UNSIGNED_INT, (void*)0);

	glDisableVertexAttribArray(shader.attribs.position);
	glDisableVertexAttribArray(shader.attribs.normal);
	glDisableVertexAttribArray(shader.attribs.texcoord);
}


void render_mesh_pos_only(const Mesh& mesh, const ShaderProgram& shader)
{
	glBindBuffer(GL_ARRAY_BUFFER, mesh.vert_buf);
	glVertexAttribPointer(shader.attribs.position, 3, GL_FLOAT, GL_FALSE, sizeof(Vert), 0);
	glEnableVertexAttribArray(shader.attribs.position);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mesh.el_buf);
	glDrawElements(GL_TRIANGLES, mesh.inds_len, GL_UNSIGNED_INT, (void*)0);
	glDisableVertexAttribArray(shader.attribs.position);
}


void render_submeshes(const Mesh& mesh, const ShaderProgram& shader)
{
	if (mesh.submeshes_len == 0)
		return;

	glBindBuffer(GL_ARRAY_BUFFER, mesh.vert_buf);

	glVertexAttribPointer(shader.attribs.position, 3, GL_FLOAT, GL_FALSE, sizeof(Vert), 0);
	glVertexAttribPointer(shader.attribs.normal, 3, GL_FLOAT, GL_FALSE, sizeof(Vert),   (void*)offsetof(struct Vert, nor));
	glVertexAttribPointer(shader.attribs.texcoord, 2, GL_FLOAT, GL_FALSE, sizeof(Vert), (void*)offsetof(struct Vert, tex));

	glEnableVertexAttribArray(shader.attribs.position);
	glEnableVertexAttribArray(shader.attribs.normal);
	glEnableVertexAttribArray(shader.attribs.texcoord);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mesh.el_buf);


	// По все подмешам
	for (int i = 0, j = 0; i < mesh.submeshes_len; i++, j += 3)
	{
		// Материал
		Material *mat = &mesh.materials[mesh.submeshes[j + 2]];

		// Биндим текстуру
		//glBindTexture(GL_TEXTURE_2D, mat->diff_texture);

		// Материал в шейдер
		glUniform4fv(shader.uniforms.diffuse_color, 1, &mat->diffuse_color[0]);
		glUniform4fv(shader.uniforms.ambient_color, 1, &mat->diffuse_color[0]);
		glUniform4fv(shader.uniforms.specular_color, 1, &mat->specular_color[0]);
		glDrawElements(GL_TRIANGLES, mesh.submeshes[j + 1], GL_UNSIGNED_INT, (void*)(sizeof(GLuint) * mesh.submeshes[j]));
	}


	glDisableVertexAttribArray(shader.attribs.position);
	glDisableVertexAttribArray(shader.attribs.normal);
	glDisableVertexAttribArray(shader.attribs.texcoord);
}


void render_submeshes(const Mesh& mesh, const ShaderProgram& shader, const char *submesh_name)
{
	if (!mesh.submeshes_names)
		return;

	for (int i = 0; i < mesh.submeshes_len; i++)
	{
		if (strcmp(mesh.submeshes_names[i], submesh_name) == 0)
		{
			glBindBuffer(GL_ARRAY_BUFFER, mesh.vert_buf);

			glVertexAttribPointer(shader.attribs.position, 3, GL_FLOAT, GL_FALSE, sizeof(Vert), 0);
			glVertexAttribPointer(shader.attribs.normal, 3, GL_FLOAT, GL_FALSE, sizeof(Vert),   (void*)offsetof(struct Vert, nor));
			glVertexAttribPointer(shader.attribs.texcoord, 2, GL_FLOAT, GL_FALSE, sizeof(Vert), (void*)offsetof(struct Vert, tex));

			glEnableVertexAttribArray(shader.attribs.position);
			glEnableVertexAttribArray(shader.attribs.normal);
			glEnableVertexAttribArray(shader.attribs.texcoord);

			glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mesh.el_buf);

			glDrawElements(GL_TRIANGLES, mesh.submeshes[3 * i + 1], GL_UNSIGNED_INT, (void*)(sizeof(GLuint) * mesh.submeshes[3 * i]));

			glDisableVertexAttribArray(shader.attribs.position);
			glDisableVertexAttribArray(shader.attribs.normal);
			glDisableVertexAttribArray(shader.attribs.texcoord);
		}
	}
}


void render_submeshes(const Mesh& mesh, const ShaderProgram& shader, int i)
{
	if (i >= mesh.submeshes_len)
		return;

	glBindBuffer(GL_ARRAY_BUFFER, mesh.vert_buf);

	glVertexAttribPointer(shader.attribs.position, 3, GL_FLOAT, GL_FALSE, sizeof(Vert), 0);
	glVertexAttribPointer(shader.attribs.normal, 3, GL_FLOAT, GL_FALSE, sizeof(Vert),   (void*)offsetof(struct Vert, nor));
	glVertexAttribPointer(shader.attribs.texcoord, 2, GL_FLOAT, GL_FALSE, sizeof(Vert), (void*)offsetof(struct Vert, tex));

	glEnableVertexAttribArray(shader.attribs.position);
	glEnableVertexAttribArray(shader.attribs.normal);
	glEnableVertexAttribArray(shader.attribs.texcoord);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mesh.el_buf);

	glDrawElements(GL_TRIANGLES, mesh.submeshes[3 * i + 1], GL_UNSIGNED_INT, (void*)(sizeof(GLuint) * mesh.submeshes[3 * i]));

	glDisableVertexAttribArray(shader.attribs.position);
	glDisableVertexAttribArray(shader.attribs.normal);
	glDisableVertexAttribArray(shader.attribs.texcoord);

}



GLuint make_framebuffer(GLuint &depthtex)
{
	if (!GLEW_EXT_framebuffer_object)
		return 0;

	GLuint fb;
	glGenFramebuffers(1, &fb);

	if (!fb)
		return 0;

	glBindFramebuffer(GL_FRAMEBUFFER, fb);

	GLuint tex;
	glGenTextures(1, &tex);
	depthtex = tex;
	glBindTexture(GL_TEXTURE_2D, tex);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT24, 1024, 1024, 0, GL_DEPTH_COMPONENT, GL_FLOAT, 0);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, depthtex, 0);

	/*GLuint depthrenderbuffer;
	glGenRenderbuffers(1, &depthrenderbuffer);
	glBindRenderbuffer(GL_RENDERBUFFER, depthrenderbuffer);
	glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT, 1024, 1024);
	glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, depthrenderbuffer);*/

	
	//GLenum DrawBuffers[1] = {GL_COLOR_ATTACHMENT0};
	//glDrawBuffers(1, DrawBuffers);
	glDrawBuffer(GL_NONE);
	glReadBuffer(GL_NONE);

	GLenum res = glCheckFramebufferStatus(GL_FRAMEBUFFER);

	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	glBindTexture(GL_TEXTURE_2D, 0);

	if (res != GL_FRAMEBUFFER_COMPLETE)
		return 0;

	return fb;
}







#include <vector>

void* loadBitmap(const char* FilePath, GLsizei& width, GLsizei& height)
{
    std::fstream hFile(FilePath, std::ios::in | std::ios::binary);
    if (!hFile.is_open()) throw std::invalid_argument("Error: File Not Found.");

    hFile.seekg(0, std::ios::end);
    std::streampos Length = hFile.tellg();
    hFile.seekg(0, std::ios::beg);
    
	unsigned char FileInfo[54];

    hFile.read((char*)FileInfo, 54);

    if(FileInfo[0] != 'B' && FileInfo[1] != 'M')
    {
        hFile.close();
        throw std::invalid_argument("Error: Invalid File Format. Bitmap Required.");
    }

    if (FileInfo[28] != 24 && FileInfo[28] != 32)
    {
        hFile.close();
        throw std::invalid_argument("Error: Invalid File Format. 24 or 32 bit Image Required.");
    }


	short BitsPerPixel = 0;
    BitsPerPixel = FileInfo[28];
    width = FileInfo[18] + (FileInfo[19] << 8);
    height = FileInfo[22] + (FileInfo[23] << 8);
    unsigned int PixelsOffset = FileInfo[10] + (FileInfo[11] << 8);
    unsigned int size = ((width * BitsPerPixel + 31) / 32) * 4 * height;
   
	if (PixelsOffset == 0)
		PixelsOffset = 54;

	char *data = new char[size];

    hFile.seekg (PixelsOffset, std::ios::beg);
    hFile.read(data, size);
    hFile.close();

	return (void*)data;
}




void convertOBJ(const OBJModel& obj, Mesh &mesh, float scalex, float scaley, float scalez)
{
	// Простая конвертация модели из OBJ формата во внутренний формат
	// Дублируются вершины, не использовать


	mesh.submeshes_len = obj.groups.size();
	mesh.submeshes = new GLint[3 * mesh.submeshes_len];
	mesh.submeshes_names = new char*[mesh.submeshes_len];

	int total_inds = 0;

	for (int i = 0; i < mesh.submeshes_len; i++)
		total_inds += obj.groups[i]->vertexIndices.size();

	mesh.vert_len = total_inds;
	mesh.inds_len = total_inds;
	mesh.verts = new Vert[total_inds];
	mesh.inds = new GLuint[total_inds];


	printf("%d Kb\n", (sizeof(Vert)*total_inds + 4 * total_inds)/ 1024);


	for (int i = 0, ind = 0; i < mesh.submeshes_len; i++)
	{
		OBJGroup *g = obj.groups[i];

		int start_ind = ind;
		mesh.submeshes[3 * i] = start_ind;
		
		mesh.submeshes_names[i] = new char[strlen(g->name) + 1];
		strcpy(mesh.submeshes_names[i], g->name);

		for (size_t j = 0; j < g->vertexIndices.size(); j++)
		{
			unsigned int index = g->vertexIndices[j];
			vert3 vertex = obj.v[index - 1];

			index = g->uvIndices[j];
			vert2 uv = obj.vt[index - 1];
			
			index = g->normalIndices[j];
			vert3 norm = obj.vn[index - 1];
			
			Vert vert;
			vert.pos[0] = vertex.x * scalex;
			vert.pos[1] = vertex.y * scaley;
			vert.pos[2] = vertex.z * scalez;

			vert.nor[0] = norm.x;
			vert.nor[1] = norm.y;
			vert.nor[2] = norm.z;

			vert.tex[0] = uv.x;
			vert.tex[1] = uv.y;

			mesh.verts[ind] = vert;
			mesh.inds[ind] = ind++;
		}

		mesh.submeshes[3 * i + 1] = ind - start_ind;
		mesh.submeshes[3 * i + 2] = g->mat_index;
	}


	mesh.materials = 0;

	if (obj.mtllib.materials.size() > 0)
	{
		mesh.materials = new Material[obj.mtllib.materials.size()];

		for (size_t i = 0; i < obj.mtllib.materials.size(); i++)
		{
			mesh.materials[i].diffuse_color = glm::vec4(obj.mtllib.materials[i].Kd.x, 
				obj.mtllib.materials[i].Kd.y, 
				obj.mtllib.materials[i].Kd.z, obj.mtllib.materials[i].d);

			mesh.materials[i].ambient_color = glm::vec4(obj.mtllib.materials[i].Ka.x, 
				obj.mtllib.materials[i].Ka.y, 
				obj.mtllib.materials[i].Ka.z, obj.mtllib.materials[i].d);

			mesh.materials[i].specular_color = glm::vec4(obj.mtllib.materials[i].Ks.x, 
				obj.mtllib.materials[i].Ks.y, 
				obj.mtllib.materials[i].Ks.z, 1.0f);

			mesh.materials[i].shininess = obj.mtllib.materials[i].Ns; 
			mesh.materials[i].alpha = obj.mtllib.materials[i].d; 

			mesh.materials[i].diff_texture = 0;

			if (strcmp(obj.mtllib.materials[i].map_Kd, "") != 0)
			{
				strcpy(mesh.materials[i].map_Kd, obj.mtllib.materials[i].map_Kd);

				bool texFound = false;
				for (size_t j = 0; j < i; j++)
				{
					if (strcmp(mesh.materials[j].map_Kd, mesh.materials[i].map_Kd) == 0)
					{
						texFound = true;
						mesh.materials[i].diff_texture = mesh.materials[j].diff_texture;
					}
				}

				if (!texFound)
				{
					mesh.materials[i].diff_texture = make_texture(mesh.materials[i].map_Kd);
				}

			}

			
			strcpy(mesh.materials[i].name, obj.mtllib.materials[i].name);
		}
	}

	

}




void convertOBJIndexed(const OBJModel& obj, Mesh &mesh, float scalex, float scaley, float scalez)
{
	// Конвертация модели из OBJ формата во внутренний формат
	// Преобразование многих индексных массивов в один

	// Кол-во подмешей = кол-во групп в OBJ
	mesh.submeshes_len = obj.groups.size();
	mesh.submeshes = new GLint[3 * mesh.submeshes_len];
	mesh.submeshes_names = new char*[mesh.submeshes_len];

	// Сколько будет индексов известно - столько же, сколько всего в OBJ файле
	int total_inds = 0;
	for (int i = 0; i < mesh.submeshes_len; i++)
		total_inds += obj.groups[i]->vertexIndices.size();
	
	mesh.inds_len = total_inds;
	mesh.inds = new GLuint[total_inds];

	// А вот количество вершин пока неизвестно, поэтому создаем для вершин вектор
	std::vector<Vert> vs;

	int reuse = 0;
	// Текущее положение в индекс-буфере
	int pi = 0;

	// Текущее положение в буфере вершин
	int pv = 0;

	// Цикл по всем подмешам
	for (int i = 0; i < mesh.submeshes_len; i++)
	{
		OBJGroup *g = obj.groups[i];

		// Сохраняем смещение подмеша
		int start_pi = pi;
		mesh.submeshes[3 * i] = start_pi;
		
		// Копируем название подмеша
		mesh.submeshes_names[i] = new char[strlen(g->name) + 1];
		strcpy(mesh.submeshes_names[i], g->name);


		// По всем индексам текущего подмеша
		for (size_t j = 0; j < g->vertexIndices.size(); j++)
		{
			// Формируем вершину из нескольких индексов

			unsigned int index = g->vertexIndices[j];
			vert3 pos = obj.v[index - 1];

			index = g->uvIndices[j];
			vert2 tex = obj.vt[index - 1];
			
			index = g->normalIndices[j];
			vert3 norm = obj.vn[index - 1];
			
			Vert vert;
			vert.pos[0] = pos.x * scalex;
			vert.pos[1] = pos.y * scaley;
			vert.pos[2] = pos.z * scalez;

			vert.nor[0] = norm.x;
			vert.nor[1] = norm.y;
			vert.nor[2] = norm.z;

			vert.tex[0] = tex.x;
			vert.tex[1] = tex.y;

			
			// Пробуем найти такую вершину со всеми аттрибутами среди уже сформированных

			bool vert_found = false;
			for (int k = 0; k < pv; k++)
			{

				Vert cv = vs[k];
				if (vert.pos[0] == cv.pos[0] &&
					vert.pos[1] == cv.pos[1] &&
					vert.pos[2] == cv.pos[2] &&
					vert.nor[0] == cv.nor[0] &&
					vert.nor[1] == cv.nor[1] &&
					vert.nor[2] == cv.nor[2] &&
					vert.tex[0] == cv.tex[0] &&
					vert.tex[1] == cv.tex[1])
				{
					// Если нашли - используем ее индекс
					mesh.inds[pi++] = k;
					vert_found = true;
					reuse++;
					break;
				}
			}

			if (!vert_found)
			{
				// Иначе добавляем новую
				vs.push_back(vert);
				pv++;
				mesh.inds[pi++] = pv - 1;
				
			}

		}

		// Сохраняем кол-во индексов в подмеше
		mesh.submeshes[3 * i + 1] = pi - start_pi;
		// И материал
		mesh.submeshes[3 * i + 2] = g->mat_index;
	}


	mesh.vert_len = vs.size();
	mesh.verts = new Vert[mesh.vert_len];
	memcpy(mesh.verts, vs.data(), mesh.vert_len * sizeof(Vert));

	vs.clear();

	mesh.materials = 0;

	if (obj.mtllib.materials.size() > 0)
	{
		mesh.materials = new Material[obj.mtllib.materials.size()];

		for (size_t i = 0; i < obj.mtllib.materials.size(); i++)
		{
			mesh.materials[i].diffuse_color = glm::vec4(obj.mtllib.materials[i].Kd.x, 
				obj.mtllib.materials[i].Kd.y, 
				obj.mtllib.materials[i].Kd.z, obj.mtllib.materials[i].d);

			mesh.materials[i].ambient_color = glm::vec4(obj.mtllib.materials[i].Ka.x, 
				obj.mtllib.materials[i].Ka.y, 
				obj.mtllib.materials[i].Ka.z, obj.mtllib.materials[i].d);

			mesh.materials[i].specular_color = glm::vec4(obj.mtllib.materials[i].Ks.x, 
				obj.mtllib.materials[i].Ks.y, 
				obj.mtllib.materials[i].Ks.z, 1.0f);

			mesh.materials[i].shininess = obj.mtllib.materials[i].Ns; 
			mesh.materials[i].alpha = obj.mtllib.materials[i].d; 

			mesh.materials[i].diff_texture = 0;

			if (strcmp(obj.mtllib.materials[i].map_Kd, "") != 0)
			{
				strcpy(mesh.materials[i].map_Kd, obj.mtllib.materials[i].map_Kd);

				bool texFound = false;
				for (size_t j = 0; j < i; j++)
				{
					if (strcmp(mesh.materials[j].map_Kd, mesh.materials[i].map_Kd) == 0)
					{
						texFound = true;
						mesh.materials[i].diff_texture = mesh.materials[j].diff_texture;
					}
				}

				if (!texFound)
				{
					mesh.materials[i].diff_texture = make_texture(mesh.materials[i].map_Kd);
				}

			}

			
			strcpy(mesh.materials[i].name, obj.mtllib.materials[i].name);
		}
	}

	


	// Память под геометрию модели
	printf("Model %d Kb %d\n", (sizeof(Vert) * mesh.vert_len + 4 * mesh.inds_len)/ 1024, reuse);

}




const GLenum cubemap_sides[] = { 
	GL_TEXTURE_CUBE_MAP_POSITIVE_X, GL_TEXTURE_CUBE_MAP_NEGATIVE_X, 
	GL_TEXTURE_CUBE_MAP_POSITIVE_Y, GL_TEXTURE_CUBE_MAP_NEGATIVE_Y,
	GL_TEXTURE_CUBE_MAP_POSITIVE_Z, GL_TEXTURE_CUBE_MAP_NEGATIVE_Z };


struct SkyBox
{
	GLuint cubemap;
	Mesh box;
};




GLuint make_cubemap(const char** sides)
{
	GLuint texture;
	glGenTextures(1, &texture);
	glBindTexture(GL_TEXTURE_CUBE_MAP, texture);

	GLsizei width, height, comp;
	for (int i = 0; i < 6; i++)
	{
		stbi_uc *data = stbi_load(sides[i], &width, &height, &comp, 3);
		if (data)
		{
			glTexImage2D(cubemap_sides[i], 0, GL_RGB8, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, (void*)data);
			stbi_image_free(data);
		}
		
	}


	glTexParameteri (GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri (GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri (GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
	glTexParameteri (GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri (GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

	return texture;
}


void make_box(Mesh &mesh, GLfloat sizex, GLfloat sizey, GLfloat sizez, bool flipNormals)
{
	//mesh.vert_len = 8;
	//mesh.verts = new Vert[8];
	//mesh.inds_len = 36;
	//mesh.inds = new GLuint[36];

	//mesh.verts[0].pos[0] = 1.0f;  mesh.verts[0].pos[1] = -1.0f; mesh.verts[0].pos[2] = -1.0f;
	//mesh.verts[1].pos[0] = 1.0f;  mesh.verts[1].pos[1] = -1.0f; mesh.verts[1].pos[2] = 1.0f;
	//mesh.verts[2].pos[0] = -1.0f; mesh.verts[2].pos[1] = -1.0f; mesh.verts[2].pos[2] = 1.0f;
	//mesh.verts[3].pos[0] = -1.0f; mesh.verts[3].pos[1] = -1.0f; mesh.verts[3].pos[2] = -1.0f;
	//mesh.verts[4].pos[0] = 1.0f;  mesh.verts[4].pos[1] = 1.0f;  mesh.verts[4].pos[2] = -1.0f;
	//mesh.verts[5].pos[0] = 1.0f;  mesh.verts[5].pos[1] = 1.0f;  mesh.verts[5].pos[2] = 1.0f;
	//mesh.verts[6].pos[0] = -1.0f; mesh.verts[6].pos[1] = 1.0f;  mesh.verts[6].pos[2] = 1.0f;
	//mesh.verts[7].pos[0] = -1.0f; mesh.verts[7].pos[1] = 1.0f;  mesh.verts[7].pos[2] = -1.0f;





	OBJModel obj;
	bool res = loadOBJ("Data\\box.obj", obj);
	
	convertOBJIndexed(obj, mesh, sizex, sizey, sizez);

	if (flipNormals)
	{
		for (int i = 0; i < mesh.vert_len; i++)
		{
			mesh.verts[i].nor[0] *= -1.0f;
			mesh.verts[i].nor[1] *= -1.0f;
			mesh.verts[i].nor[2] *= -1.0f;
		}
	}

	make_mesh_buffers(mesh);


	
}





void make_skybox(SkyBox &skybox, const char** sides, GLfloat size)
{
	skybox.cubemap = make_cubemap(sides);
	make_box(skybox.box, size, size, size, true);


}
