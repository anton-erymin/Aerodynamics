#pragma once

#include "graphics.h"


#define		TERR_BORDER_LEFT	0
#define		TERR_BORDER_RIGHT	1
#define		TERR_BORDER_TOP		2
#define		TERR_BORDER_BOTTOM	3

#define		TERR_BORDER_TYPE_NORMAL		0
#define		TERR_BORDER_TYPE_TRANS		1

struct Chunk
{
	// Смещения в индексном буфере начала индексов чанка
	GLuint chunkOffset;

	// Текущий уровень чанка
	unsigned char LOD;

	// Типы границ чанка
	unsigned char bordersTypes[4];
};

struct Terrain
{
	// Размер в количестве точек
	GLuint size;

	// Размер в пространстве
	GLfloat x0, x1, chunkLength;

	int chunkSize;
	
	// Размер буфера вершин
	GLsizei vert_len;
	// Размер буфера индексов
	GLsizei inds_len;

	// Данные вершин
	Vert *verts;
	// Индексы
	GLuint *inds;

	// Смещения разных уровней в индексном буфере
	GLuint *LODoffsets;
	// Кол-во индексов в разных уровнях
	GLsizei *LODlengths;
	// Смещения разных уровней в индексном буфере
	GLuint LODbordersOffsets[10][4][2];
	// Кол-во индексов в разных уровнях
	GLsizei LODbordersLengths[10][4][2];

	// Размер стороны в чанках
	int sizeInChunks;
	// Всего чанков
	int chunkNum;
	Chunk *chunks;

	// Хэндлы буферов
	GLuint vert_buf, el_buf;


	// Массивы указания нужных индексов для отрисовки с помощью glMultiDrawElements
	GLuint *indPointersArray;
	GLsizei *countArray;

	// Текущий чанк
	int curChunk;

	int maxLOD;
};





void terrain_gen_chunkindexes(Terrain &terr)
{
	// Размер чанка
	int chunkSize = 65;
	terr.chunkSize = chunkSize;

	// Максимальный уровень детализации
	terr.maxLOD = 6;

	std::vector<GLuint> chunkInds;

	
	terr.LODoffsets = new GLuint[terr.maxLOD];
	terr.LODlengths = new GLsizei[terr.maxLOD];

	int pi = 0, start_pi;

	// Генерируем индексы ЛОДов одного чанка
	for (int lod = 0; lod <= terr.maxLOD; lod++)
	{
		// Через сколько вершин перепрыгивать
		int stride = 1 << lod;

		start_pi = pi;

		// Сначала внутренняя область чанка, без границ
		for (int i = stride; i < chunkSize - 2 * stride; i += stride)
		{
			for (int j = stride; j < chunkSize - 2 * stride; j += stride)
			{
				int pv = i * terr.size + j;
				int pv2 = pv + stride * terr.size;

				chunkInds.push_back(pv);
				chunkInds.push_back(pv + stride);
				chunkInds.push_back(pv2);
				chunkInds.push_back(pv2);
				chunkInds.push_back(pv + stride);
				chunkInds.push_back(pv2 + stride);

				// Добавлено 6 индексов
				pi += 6;
			}
		}

		// Запоминаем смещение и длину данного ЛОДа
		terr.LODoffsets[lod] = sizeof(GLuint) * start_pi;
		terr.LODlengths[lod] = pi - start_pi;


		// Затем генерируем индексы для разных типов границ со всех сторон

		// Слева
		int i = 0;

		// Обычная граница
		start_pi = pi;
		for (int j = 0; j < chunkSize - 2 * stride; j += 2 * stride)
		{
			int pv = i * terr.size + j;
			int pv2 = pv + stride * terr.size;

			chunkInds.push_back(pv);
			chunkInds.push_back(pv + stride);
			chunkInds.push_back(pv2 + stride);

			chunkInds.push_back(pv + stride);
			chunkInds.push_back(pv + 2 * stride);
			chunkInds.push_back(pv2 + stride);
			

			pi += 6;

			if (j < chunkSize - 3 * stride)
			{
				chunkInds.push_back(pv + 2 * stride);
				chunkInds.push_back(pv2 + 2 * stride);
				chunkInds.push_back(pv2 + stride);

				chunkInds.push_back(pv + 2 * stride);
				chunkInds.push_back(pv2 + 3 * stride);
				chunkInds.push_back(pv2 + 2 * stride);

				pi += 6;
			}
		}
		terr.LODbordersOffsets[lod][TERR_BORDER_LEFT][TERR_BORDER_TYPE_NORMAL] = sizeof(GLuint) * start_pi;
		terr.LODbordersLengths[lod][TERR_BORDER_LEFT][TERR_BORDER_TYPE_NORMAL] = pi - start_pi;



		// Проряженная граница
		start_pi = pi;
		for (int j = 0; j < chunkSize - 2 * stride; j += 2 * stride)
		{
			int pv = i * terr.size + j;
			int pv2 = pv + stride * terr.size;

			chunkInds.push_back(pv);
			chunkInds.push_back(pv + 2 * stride);
			chunkInds.push_back(pv2 + stride);

			pi += 3;

			if (j < chunkSize - 3 * stride)
			{
				chunkInds.push_back(pv + 2 * stride);
				chunkInds.push_back(pv2 + 2 * stride);
				chunkInds.push_back(pv2 + stride);

				chunkInds.push_back(pv + 2 * stride);
				chunkInds.push_back(pv2 + 3 * stride);
				chunkInds.push_back(pv2 + 2 * stride);

				pi += 6;
			}
		}
		terr.LODbordersOffsets[lod][TERR_BORDER_LEFT][TERR_BORDER_TYPE_TRANS] = sizeof(GLuint) * start_pi;
		terr.LODbordersLengths[lod][TERR_BORDER_LEFT][TERR_BORDER_TYPE_TRANS] = pi - start_pi;




		// Справа
		i = chunkSize - 1;

		// Обычная граница
		start_pi = pi;
		for (int j = 0; j < chunkSize - 2 * stride; j += 2 * stride)
		{
			int pv = i * terr.size + j;
			int pv2 = pv - stride * terr.size;

			chunkInds.push_back(pv);
			chunkInds.push_back(pv2 + stride);
			chunkInds.push_back(pv + stride);

			chunkInds.push_back(pv + stride);
			chunkInds.push_back(pv2 + stride);
			chunkInds.push_back(pv + 2 * stride);
			
			pi += 6;

			if (j < chunkSize - 3 * stride)
			{
				chunkInds.push_back(pv + 2 * stride);
				chunkInds.push_back(pv2 + stride);
				chunkInds.push_back(pv2 + 2 * stride);

				chunkInds.push_back(pv + 2 * stride);
				chunkInds.push_back(pv2 + 2 * stride);
				chunkInds.push_back(pv2 + 3 * stride);

				pi += 6;
			}
		}
		terr.LODbordersOffsets[lod][TERR_BORDER_RIGHT][TERR_BORDER_TYPE_NORMAL] = sizeof(GLuint) * start_pi;
		terr.LODbordersLengths[lod][TERR_BORDER_RIGHT][TERR_BORDER_TYPE_NORMAL] = pi - start_pi;


		// Проряженная граница
		start_pi = pi;
		for (int j = 0; j < chunkSize - 2 * stride; j += 2 * stride)
		{
			int pv = i * terr.size + j;
			int pv2 = pv - stride * terr.size;

			chunkInds.push_back(pv);
			chunkInds.push_back(pv2 + stride);
			chunkInds.push_back(pv + 2 * stride);
			

			pi += 3;

			if (j < chunkSize - 3 * stride)
			{
				chunkInds.push_back(pv + 2 * stride);
				chunkInds.push_back(pv2 + stride);
				chunkInds.push_back(pv2 + 2 * stride);
				

				chunkInds.push_back(pv + 2 * stride);
				chunkInds.push_back(pv2 + 2 * stride);
				chunkInds.push_back(pv2 + 3 * stride);

				pi += 6;
			}
		}
		terr.LODbordersOffsets[lod][TERR_BORDER_RIGHT][TERR_BORDER_TYPE_TRANS] = sizeof(GLuint) * start_pi;
		terr.LODbordersLengths[lod][TERR_BORDER_RIGHT][TERR_BORDER_TYPE_TRANS] = pi - start_pi;


		// Сверху
		int j = 0;

		// Обычная граница
		start_pi = pi;
		for (int i = 0; i < chunkSize - 2 * stride; i += 2 * stride)
		{
			int pv = i * terr.size + j;
			int pv2 = pv + stride * terr.size;
			int pv3 = pv + 2 * stride * terr.size;
			int pv4 = pv + 3 * stride * terr.size;

			chunkInds.push_back(pv);
			chunkInds.push_back(pv2 + stride);
			chunkInds.push_back(pv2);

			chunkInds.push_back(pv2 + stride);
			chunkInds.push_back(pv3);
			chunkInds.push_back(pv2);

			pi += 6;

			if (i < chunkSize - 3 * stride)
			{
				chunkInds.push_back(pv2 + stride);
				chunkInds.push_back(pv3 + stride);
				chunkInds.push_back(pv3);

				chunkInds.push_back(pv3 + stride);
				chunkInds.push_back(pv4 + stride);
				chunkInds.push_back(pv3);

				pi += 6;
			}
		}
		terr.LODbordersOffsets[lod][TERR_BORDER_TOP][TERR_BORDER_TYPE_NORMAL] = sizeof(GLuint) * start_pi;
		terr.LODbordersLengths[lod][TERR_BORDER_TOP][TERR_BORDER_TYPE_NORMAL] = pi - start_pi;

		// Проряженная граница
		start_pi = pi;
		for (int i = 0; i < chunkSize - 2 * stride; i += 2 * stride)
		{
			int pv = i * terr.size + j;
			int pv2 = pv + stride * terr.size;
			int pv3 = pv + 2 * stride * terr.size;
			int pv4 = pv + 3 * stride * terr.size;

			chunkInds.push_back(pv);
			chunkInds.push_back(pv2 + stride);
			chunkInds.push_back(pv3);

			pi += 3;

			if (i < chunkSize - 3 * stride)
			{
				chunkInds.push_back(pv2 + stride);
				chunkInds.push_back(pv3 + stride);
				chunkInds.push_back(pv3);

				chunkInds.push_back(pv3 + stride);
				chunkInds.push_back(pv4 + stride);
				chunkInds.push_back(pv3);

				pi += 6;
			}
		}
		terr.LODbordersOffsets[lod][TERR_BORDER_TOP][TERR_BORDER_TYPE_TRANS] = sizeof(GLuint) * start_pi;
		terr.LODbordersLengths[lod][TERR_BORDER_TOP][TERR_BORDER_TYPE_TRANS] = pi - start_pi;


		// Снизу
		j = chunkSize - 1;

		// Обычная граница
		start_pi = pi;
		for (int i = 0; i < chunkSize - 2 * stride; i += 2 * stride)
		{
			int pv = i * terr.size + j;
			int pv2 = pv + stride * terr.size;
			int pv3 = pv + 2 * stride * terr.size;
			int pv4 = pv + 3 * stride * terr.size;

			chunkInds.push_back(pv);
			chunkInds.push_back(pv2);
			chunkInds.push_back(pv2 - stride);
			
			chunkInds.push_back(pv2 - stride);
			chunkInds.push_back(pv2);
			chunkInds.push_back(pv3);

			pi += 6;

			if (i < chunkSize - 3 * stride)
			{
				chunkInds.push_back(pv2 - stride);
				chunkInds.push_back(pv3);
				chunkInds.push_back(pv3 - stride);
				
				chunkInds.push_back(pv3 - stride);
				chunkInds.push_back(pv3);
				chunkInds.push_back(pv4 - stride);
				

				pi += 6;
			}
		}
		terr.LODbordersOffsets[lod][TERR_BORDER_BOTTOM][TERR_BORDER_TYPE_NORMAL] = sizeof(GLuint) * start_pi;
		terr.LODbordersLengths[lod][TERR_BORDER_BOTTOM][TERR_BORDER_TYPE_NORMAL] = pi - start_pi;


		// Проряженная граница
		start_pi = pi;
		for (int i = 0; i < chunkSize - 2 * stride; i += 2 * stride)
		{
			int pv = i * terr.size + j;
			int pv2 = pv + stride * terr.size;
			int pv3 = pv + 2 * stride * terr.size;
			int pv4 = pv + 3 * stride * terr.size;

			chunkInds.push_back(pv);
			chunkInds.push_back(pv3);
			chunkInds.push_back(pv2 - stride);

			pi += 3;

			if (i < chunkSize - 3 * stride)
			{
				chunkInds.push_back(pv2 - stride);
				chunkInds.push_back(pv3);
				chunkInds.push_back(pv3 - stride);
				

				chunkInds.push_back(pv3 - stride);
				chunkInds.push_back(pv3);
				chunkInds.push_back(pv4 - stride);
				

				pi += 6;
			}
		}
		terr.LODbordersOffsets[lod][TERR_BORDER_BOTTOM][TERR_BORDER_TYPE_TRANS] = sizeof(GLuint) * start_pi;
		terr.LODbordersLengths[lod][TERR_BORDER_BOTTOM][TERR_BORDER_TYPE_TRANS] = pi - start_pi;

		

	} // LOD



	// Копируем индексы для остальных чанков со смещением индексов базового


	// Кол-во чанков в стороне
	terr.sizeInChunks = (terr.size - 1) / (chunkSize - 1);
	// Всего чанков
	terr.chunkNum = terr.sizeInChunks * terr.sizeInChunks;

	// Создаем индексный массив
	int numInds = chunkInds.size();
	terr.inds_len = terr.chunkNum * numInds;
	terr.inds = new GLuint[terr.inds_len];

	terr.chunks = new Chunk[terr.chunkNum];

	int offset;
	pi = 0;
	for (int i = 0; i < terr.sizeInChunks; i++)
	{
		// Смещение индексов для нового столбца
		offset = i * (chunkSize - 1) * terr.size;

		for (int j = 0; j < terr.sizeInChunks; j++)
		{
			int chunkIndex = i * terr.sizeInChunks + j;

			terr.chunks[chunkIndex].chunkOffset = sizeof(GLuint) * pi;
			terr.chunks[chunkIndex].LOD = terr.maxLOD;

			for (int k = 0; k < 4; k++)
				terr.chunks[chunkIndex].bordersTypes[k] = TERR_BORDER_TYPE_NORMAL;

			for (int k = 0; k < numInds; k++)
			{
				terr.inds[pi++] = chunkInds[k] + offset;
			}
			// Смещение индексов по вниз строке
			offset += (chunkSize - 1);

		}

		
	}


	terr.el_buf = make_buffer(GL_ELEMENT_ARRAY_BUFFER, terr.inds, terr.inds_len * sizeof(GLuint));
	delete[] terr.inds;


	terr.countArray		  = new GLsizei[5 * terr.chunkNum];
	terr.indPointersArray = new GLuint[5 * terr.chunkNum];
	
	for (int i = 0, j = 0; i < terr.chunkNum; i++, j += 5)
	{
		int lod = terr.chunks[i].LOD;
		int offset = terr.chunks[i].chunkOffset;

		terr.indPointersArray[j]	 = offset + terr.LODoffsets[lod];
		terr.countArray[j]			 = terr.LODlengths[lod];

		terr.indPointersArray[j + 1] = offset + terr.LODbordersOffsets[lod][0][terr.chunks[i].bordersTypes[0]];
		terr.indPointersArray[j + 2] = offset + terr.LODbordersOffsets[lod][1][terr.chunks[i].bordersTypes[1]];
		terr.indPointersArray[j + 3] = offset + terr.LODbordersOffsets[lod][2][terr.chunks[i].bordersTypes[2]];
		terr.indPointersArray[j + 4] = offset + terr.LODbordersOffsets[lod][3][terr.chunks[i].bordersTypes[3]];

		terr.countArray[j + 1]		 = terr.LODbordersLengths[lod][0][terr.chunks[i].bordersTypes[0]];
		terr.countArray[j + 2]		 = terr.LODbordersLengths[lod][1][terr.chunks[i].bordersTypes[1]];
		terr.countArray[j + 3]		 = terr.LODbordersLengths[lod][2][terr.chunks[i].bordersTypes[2]];
		terr.countArray[j + 4]		 = terr.LODbordersLengths[lod][3][terr.chunks[i].bordersTypes[3]];

			
	}
}



void terrain_gen_vb(Terrain &terr, char *data, GLfloat size, GLfloat maxheight, int blurCount, bool add, int map_res)
{
	// Генерация вершин

	// Выделяем память под вершины
	int res = terr.size;

	terr.vert_len = res * res;
	terr.verts = new Vert[terr.vert_len];

	
	// Расчет детализированной сетки ландшафта
 
	GLfloat x = -size * 0.5f;
	GLfloat dx = size / (res - 1);
	GLfloat dz = dx;

	terr.x0 = x;
	terr.x1 = -x;

	terr.chunkLength = dx * 64;

	int pv = 0;

	for (int i = 0; i < res; i++)
	{
		GLfloat z = -size * 0.5f;
		GLfloat curx = x + i * dx;

		for (int j = 0; j < res; j++)
		{
			GLubyte h = 0;
			if (add && (i == res - 1 || j == res - 1))
			{
				h = 0;
			}
			else h = *(data + 3 * (i * map_res + j));

			Vert v;
			v.pos[0] = curx;
			v.pos[1] = (GLfloat)h / 255.0f * maxheight;
			v.pos[2] = z + j * dz;

			v.nor[0] = 0.0f; 
			v.nor[1] = 1.0f;
			v.nor[2] = 0.0f;

			v.tex[0] = 0.0f;
			v.tex[1] = 0.0f;

			terr.verts[pv++] = v;

			//printf("(%f, %f, %f)\n", v.pos[0], v.pos[1], v.pos[2]);
		}
	}
	
	delete[] data;


	// Сглаживаем ландшафт (Blur)

	for (int iter = 0; iter < blurCount; iter++)
	{
		for (int i = 1; i < res - 1; i++)
		{
			for (int j = 1; j < res - 1; j++)
			{
				int k = i * res + j;

				terr.verts[k].pos[1] = (terr.verts[k].pos[1] + terr.verts[k - 1].pos[1] + terr.verts[k + 1].pos[1] + 
					terr.verts[k - res - 1].pos[1] + terr.verts[k - res].pos[1] + terr.verts[k - res + 1].pos[1] +
					terr.verts[k + res - 1].pos[1] + terr.verts[k + res].pos[1] + terr.verts[k + res + 1].pos[1]) / 9;
			}
		}
	}

	// Обработаем краевые точки
	for (int i = 0; i < res; i++)
	{
		terr.verts[i * res].pos[1] = terr.verts[i * res + 1].pos[1];
		terr.verts[i * res + res - 1].pos[1] = terr.verts[i * res + res - 2].pos[1];
	}
	for (int j = 0; j < res; j++)
	{
		terr.verts[j].pos[1] = terr.verts[1 * res + j].pos[1];
		terr.verts[(res - 1) * res + j].pos[1] = terr.verts[(res - 2) * res + j].pos[1];
	}


	// Нормали вершин считаем на основе самой детализированной сетки
	int numQuads = (res - 1) * (res - 1);

	// Считаем нормали треугольников и суммируем их в вершинах
	int pi = 0;

	for (int i = 0; i < res - 1; i++)
	{
		for (int j = 0; j < res - 1; j++)
		{
			pv = i * res + j;
			Vert *v1 = &terr.verts[pv];
			Vert *v2 = &terr.verts[pv + 1];
			Vert *v3 = &terr.verts[pv + res];

			glm::vec3 a(v2->pos[0] - v1->pos[0], v2->pos[1] - v1->pos[1], v2->pos[2] - v1->pos[2]);
			glm::vec3 b(v3->pos[0] - v1->pos[0], v3->pos[1] - v1->pos[1], v3->pos[2] - v1->pos[2]);
			glm::vec3 n = glm::cross(a, b);
			n = glm::normalize(n);

			v1->nor[0] += n.x;
			v1->nor[1] += n.y;
			v1->nor[2] += n.z;
			v2->nor[0] += n.x;
			v2->nor[1] += n.y;
			v2->nor[2] += n.z;
			v3->nor[0] += n.x;
			v3->nor[1] += n.y;
			v3->nor[2] += n.z;

			v1 = &terr.verts[pv + res];
			v2 = &terr.verts[pv + 1];
			v3 = &terr.verts[pv + res + 1];

			v1->nor[0] += n.x;
			v1->nor[1] += n.y;
			v1->nor[2] += n.z;
			v2->nor[0] += n.x;
			v2->nor[1] += n.y;
			v2->nor[2] += n.z;
			v3->nor[0] += n.x;
			v3->nor[1] += n.y;
			v3->nor[2] += n.z;
		}
	}

	// Нормализуем средние нормали в каждой вершине
	/*for (int i = 0; i < mesh.vert_len; i++)
	{
		glm::vec3 n(mesh.verts[i].nor[0], mesh.verts[i].nor[1], mesh.verts[i].nor[2]);
		n = glm::normalize(n);
		mesh.verts[i].nor[0] = n.x;
		mesh.verts[i].nor[1] = n.y;
		mesh.verts[i].nor[2] = n.z;
	}*/


	terr.vert_buf = make_buffer(GL_ARRAY_BUFFER, terr.verts, terr.vert_len * sizeof(Vert));
	delete[] terr.verts;
}



bool make_terrain_fromheightmap(Terrain &terr, const char *heightmap, GLfloat size, GLfloat maxheight, int blurCount)
{
	// Ландшафт из карты высот

	// Читаем карту высот

	GLsizei resx, resy;
	int comp;
	char *data = (char*)stbi_load(heightmap, &resx, &resy, &comp, 3);

	printf(stbi_failure_reason());

	if (!data)
		return false;

	// Проверяем квадратность карты и степень двойки
	if (resx != resy)
	{
		return false;
	}

	int res = resx;
	bool add = false;
	if (res & (res - 1))
	{
		if ((res - 1) & ((res - 1) - 1))
		{
			return false;
		}
	}
	else
	{
		res++;
		add = true;
	}

	
	terr.size = res;
	
	terrain_gen_vb(terr, data, size, maxheight, blurCount, add, resx);
	terrain_gen_chunkindexes(terr);


		
	printf("%d Kb verts for land\n", (sizeof(Vert) * terr.vert_len)/ 1024);
	printf("%d Kb inds for land\n", (sizeof(GLuint) * terr.inds_len)/ 1024);

	return true;
}



void render_terrain(const Terrain& terr, const ShaderProgram& shader)
{
	glBindBuffer(GL_ARRAY_BUFFER, terr.vert_buf);

	glVertexAttribPointer(shader.attribs.position, 3, GL_FLOAT, GL_FALSE, sizeof(Vert), 0);
	glVertexAttribPointer(shader.attribs.normal, 3, GL_FLOAT, GL_FALSE, sizeof(Vert),   (void*)offsetof(struct Vert, nor));
	glVertexAttribPointer(shader.attribs.texcoord, 2, GL_FLOAT, GL_FALSE, sizeof(Vert), (void*)offsetof(struct Vert, tex));

	glEnableVertexAttribArray(shader.attribs.position);
	glEnableVertexAttribArray(shader.attribs.normal);
	glEnableVertexAttribArray(shader.attribs.texcoord);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, terr.el_buf);

	glMultiDrawElements(GL_TRIANGLES, terr.countArray, GL_UNSIGNED_INT, (const void* const*)terr.indPointersArray, 5 * terr.chunkNum);


	glDisableVertexAttribArray(shader.attribs.position);
	glDisableVertexAttribArray(shader.attribs.normal);
	glDisableVertexAttribArray(shader.attribs.texcoord);
}


void terrain_update_lods(Terrain &terr, GLfloat x, GLfloat z)
{
	// Расчет лодов и границ

	// Определяем координаты текущего чанка
	int i = static_cast<int>((x - terr.x0) / terr.chunkLength);
	int j = static_cast<int>((z - terr.x0) / terr.chunkLength);

	// Проверяем на выход за границы
	if (i < 0)
		i = 0;
	if (i >= terr.sizeInChunks)
		i = terr.sizeInChunks - 1;
	if (j < 0)
		j = 0;
	if (j >= terr.sizeInChunks)
		j = terr.sizeInChunks - 1;

	// Индекс в массиве
	int newChunk = i * terr.sizeInChunks + j;

	// Если перешли в другой чанк
	if (terr.curChunk != newChunk)
	{
		// Делаем текущим
		terr.curChunk = newChunk;
		// Самый детализированный
		terr.chunks[newChunk].LOD = 0;
		// Переходные границы со всех сторон
		terr.chunks[newChunk].bordersTypes[0] = 0;
		terr.chunks[newChunk].bordersTypes[1] = 0;
		terr.chunks[newChunk].bordersTypes[2] = 0;
		terr.chunks[newChunk].bordersTypes[3] = 0;

		// Пересчитываем лоды вокруг текущего чанка
		int k = 1;
		while (true)
		{
			// Условие выхода
			if (i - k < 0 && i + k >= terr.sizeInChunks &&
				j - k < 0 && j + k >= terr.sizeInChunks)
				break;


			int newLOD = k - 1 <= terr.maxLOD - 1 ? k - 1 : terr.maxLOD - 1;

			
			
			for (int x = i - k; x <= i + k; x++)
			{
				if (x < 0)
					continue;
				if (x >= terr.sizeInChunks)
					break;

				if (j - k < 0 && j + k >= terr.sizeInChunks)
					break;

				int z = j - k;
				if (z >= 0) 
				{
					int ci = x * terr.sizeInChunks + z;
					if (ci >= 0 && ci < terr.chunkNum)
					{
						terr.chunks[ci].LOD = newLOD;

						for (int b = 0; b < 4; b++)
						{
							terr.chunks[ci].bordersTypes[b] = 0;
						}

						if (newLOD < terr.maxLOD - 1)
						{
							terr.chunks[ci].bordersTypes[TERR_BORDER_TOP] = TERR_BORDER_TYPE_TRANS;

							if (x == i - k)
								terr.chunks[ci].bordersTypes[TERR_BORDER_LEFT] = TERR_BORDER_TYPE_TRANS;

							if (x == i + k)
								terr.chunks[ci].bordersTypes[TERR_BORDER_RIGHT] = TERR_BORDER_TYPE_TRANS;
						}
					}
				}

				z = j + k;
				if (z < terr.sizeInChunks)
				{
					int ci = x * terr.sizeInChunks + z;
					if (ci >= 0 && ci < terr.chunkNum)
					{
						terr.chunks[ci].LOD = newLOD;

						for (int b = 0; b < 4; b++)
						{
							terr.chunks[ci].bordersTypes[b] = 0;
						}


						if (newLOD < terr.maxLOD - 1)
						{
							terr.chunks[ci].bordersTypes[TERR_BORDER_BOTTOM] = TERR_BORDER_TYPE_TRANS;
						
							if (x == i - k)
								terr.chunks[ci].bordersTypes[TERR_BORDER_LEFT] = TERR_BORDER_TYPE_TRANS;
						
							if (x == i + k)
								terr.chunks[ci].bordersTypes[TERR_BORDER_RIGHT] = TERR_BORDER_TYPE_TRANS;
						}
					}
				}
			}

			for (int z = j - k + 1; z <= j + k - 1; z++)
			{
				if (z < 0)
					continue;
				if (z >= terr.sizeInChunks)
					break;

				if (i - k < 0 && i + k >= terr.sizeInChunks)
					break;

				int x = i - k;
				if (x >= 0) 
				{
					int ci = x * terr.sizeInChunks + z;
					if (ci >= 0 && ci < terr.chunkNum)
					{
						terr.chunks[ci].LOD = newLOD;

						for (int b = 0; b < 4; b++)
						{
							terr.chunks[ci].bordersTypes[b] = 0;
						}

						if (newLOD < terr.maxLOD - 1)
						{
							terr.chunks[ci].bordersTypes[TERR_BORDER_LEFT] = TERR_BORDER_TYPE_TRANS;
						}
						
					}
				}

				x = i + k;
				if (x < terr.sizeInChunks)
				{
					int ci = x * terr.sizeInChunks + z;
					if (ci >= 0 && ci < terr.chunkNum)
					{
						terr.chunks[ci].LOD = newLOD;

						for (int b = 0; b < 4; b++)
						{
							terr.chunks[ci].bordersTypes[b] = 0;
						}

						if (newLOD < terr.maxLOD - 1)
						{
							terr.chunks[ci].bordersTypes[TERR_BORDER_RIGHT] = TERR_BORDER_TYPE_TRANS;
						}
						
					}
				}
			}
			k++;
		}

		int num = 0;

		// Переформировываем массивы для glMultiDrawElements
		for (int i = 0, j = 0; i < terr.chunkNum; i++, j += 5)
		{
			int lod = terr.chunks[i].LOD;
			int offset = terr.chunks[i].chunkOffset;

			terr.indPointersArray[j]	 = offset + terr.LODoffsets[lod];
			terr.countArray[j]			 = terr.LODlengths[lod];

			terr.indPointersArray[j + 1] = offset + terr.LODbordersOffsets[lod][0][terr.chunks[i].bordersTypes[0]];
			terr.indPointersArray[j + 2] = offset + terr.LODbordersOffsets[lod][1][terr.chunks[i].bordersTypes[1]];
			terr.indPointersArray[j + 3] = offset + terr.LODbordersOffsets[lod][2][terr.chunks[i].bordersTypes[2]];
			terr.indPointersArray[j + 4] = offset + terr.LODbordersOffsets[lod][3][terr.chunks[i].bordersTypes[3]];

			terr.countArray[j + 1]		 = terr.LODbordersLengths[lod][0][terr.chunks[i].bordersTypes[0]];
			terr.countArray[j + 2]		 = terr.LODbordersLengths[lod][1][terr.chunks[i].bordersTypes[1]];
			terr.countArray[j + 3]		 = terr.LODbordersLengths[lod][2][terr.chunks[i].bordersTypes[2]];
			terr.countArray[j + 4]		 = terr.LODbordersLengths[lod][3][terr.chunks[i].bordersTypes[3]];

			num += (terr.countArray[j] + terr.countArray[j + 1] + terr.countArray[j + 2] + terr.countArray[j + 3] + terr.countArray[j + 4]) / 3;
		}

		printf("Terrain drawing %d triangles\n", num);

	}


}