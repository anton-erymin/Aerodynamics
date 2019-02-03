#pragma once

#include <fstream>
#include <vector>

// Для загрузки модели

struct vert3
{
	float x, y, z;
};

struct vert2
{
	float x, y;
};



struct MTLMat
{
	char name[32];

	vert3 Kd;
	vert3 Ka;
	vert3 Ks;
	vert3 Ke;
	float d;
	float Ns;
	int illum;
	char map_Kd[256];
	char map_bump[256];
};

struct MTLLib
{
	std::vector<MTLMat> materials;
};


struct OBJGroup
{
	char name[32];
	short mat_index;
	char matname[32];

	std::vector<unsigned int> vertexIndices;
	std::vector<unsigned int> uvIndices;
	std::vector<unsigned int> normalIndices;
};

struct OBJModel
{
	std::vector<vert3> v;
	std::vector<vert2> vt;
	std::vector<vert3> vn;

	std::vector<OBJGroup*> groups;

	MTLLib mtllib;
};





bool loadOBJ(const char *, OBJModel &obj);
bool loadMTL(const char *fname, OBJModel &obj);



inline void skip(int& ptr)
{
	while (*(char*)ptr == ' ' || *(char*)ptr == '\t' || *(char*)ptr == '\n') ptr++;
}


// Загрузчик OBJ моделей
bool loadOBJ(const char *fname, OBJModel &obj)
{
	char path[256];
	int len = strrchr(fname, '\\') - fname + 1;
	strncpy(path, fname, len);
	path[len] = 0;


	OBJGroup *g = 0;
	


	std::fstream hFile(fname, std::ios::in | std::ios::binary);
    if (!hFile.is_open()) throw std::invalid_argument("Error: File Not Found.");


	while (!hFile.eof())
	{
		char line[512];
		hFile.getline(line, 512);
		
		char word[128];


		char *p = line, *st;

		skip((int&)p);
		st = p;
		while (*p != ' ' && *p != '\t' && *p != '\n') p++;
		strncpy(word, st, p - st);
		word[p - st] = 0;




		if (strcmp(word, "#") == 0)
			continue;

		if (strcmp(word, "mtllib") == 0)
		{
			skip((int&)p);
			char mtlfile[256];
			strcpy(mtlfile, path);
			strcat(mtlfile, p);

			loadMTL(mtlfile, obj);

			continue;
		}


		if (strcmp(word, "g") == 0)
		{
			if (g)
			{
				obj.groups.push_back(g);
			}

			skip((int&)p);
			g = new OBJGroup;
			strcpy(g->name, p);
			strcpy(g->matname, "");
			g->mat_index = -1;

			continue;
		}

		if (strcmp(word, "usemtl") == 0)
		{
			if (!g)
			{
				g = new OBJGroup;
				strcpy(g->name, "unnamed");
				
				g->mat_index = -1;
			}
			skip((int&)p);
			strcpy(g->matname, p);

			continue;
		}


		if (strcmp(word, "v") == 0)
		{
			vert3 vertex;
			sscanf(p, "%f %f %f\n", &vertex.x, &vertex.y, &vertex.z);
			obj.v.push_back(vertex);
		}
		else if (strcmp(word, "vt") == 0)
		{
			vert2 uv;
			sscanf(p, "%f %f\n", &uv.x, &uv.y);
			obj.vt.push_back(uv);
		}
		else if (strcmp(word, "vn") == 0)
		{
			vert3 normal;
			sscanf(p, "%f %f %f\n", &normal.x, &normal.y, &normal.z);
			obj.vn.push_back(normal);
		}
		else if (strcmp(word, "f") == 0)
		{
			std::string vertex1, vertex2, vertex3;
			unsigned int vertexIndex[3], uvIndex[3], normalIndex[3];
			int matches = sscanf(p, "%d/%d/%d %d/%d/%d %d/%d/%d\n", &vertexIndex[0], &uvIndex[0], &normalIndex[0], &vertexIndex[1], &uvIndex[1], &normalIndex[1], &vertexIndex[2], &uvIndex[2], &normalIndex[2]);
			if (matches != 9)
			{
				printf("File can't be read by our simple parser : ( Try exporting with other options\n");
				//return false;
				matches = sscanf(p, "%d////%d %d////%d %d////%d\n", &vertexIndex[0], &normalIndex[0], &vertexIndex[1], &normalIndex[1], &vertexIndex[2], &normalIndex[2]);
				if (matches != 6)
				{
					printf("File can't be read by our simple parser : ( Try exporting with other options\n");	
				}
			}


			if (!g)
			{
				g = new OBJGroup;
				strcpy(g->name, "unnamed");
				strcpy(g->matname, "");
				g->mat_index = -1;
			}

			g->vertexIndices.push_back(vertexIndex[0]);
			g->vertexIndices.push_back(vertexIndex[1]);
			g->vertexIndices.push_back(vertexIndex[2]);
			g->uvIndices.push_back(uvIndex[0]);
			g->uvIndices.push_back(uvIndex[1]);
			g->uvIndices.push_back(uvIndex[2]);
			g->normalIndices.push_back(normalIndex[0]);
			g->normalIndices.push_back(normalIndex[1]);
			g->normalIndices.push_back(normalIndex[2]);
		}
	}

	if (g)
	{
		obj.groups.push_back(g);
	}


	if (obj.mtllib.materials.size() > 0)
	{
		for (size_t i = 0; i < obj.groups.size(); i++)
		{
			unsigned short matIndex = 0;
			bool matFound = false;
			for (; matIndex < obj.mtllib.materials.size(); matIndex++)
			{
				if (strcmp(obj.groups[i]->matname, obj.mtllib.materials[matIndex].name) == 0)
				{
					matFound = true;
					break;
				}
			}

			if (matFound)
			{
				obj.groups[i]->mat_index = matIndex;
			}

		}
	}


	return true;
}




bool loadMTL(const char *fname, OBJModel &obj)
{
	char path[256];
	int len = strrchr(fname, '\\') - fname + 1;
	strncpy(path, fname, len);
	path[len] = 0;


	bool first = true;

	MTLMat m;
	strcpy(m.name, "");
	strcpy(m.map_Kd, "");
	strcpy(m.map_bump, "");
	

	std::fstream hFile(fname, std::ios::in | std::ios::binary);
    if (!hFile.is_open()) throw std::invalid_argument("Error: File Not Found.");


	while (!hFile.eof())
	{
		char line[512];
		hFile.getline(line, 512);
		
		char word[128];


		char *p = line, *st;

		skip((int&)p);
		st = p;
		while (*p != ' ' && *p != '\t' && *p != '\n') p++;
		strncpy(word, st, p - st);
		word[p - st] = 0;




		if (strcmp(word, "#") == 0)
			continue;


		if (strcmp(word, "newmtl") == 0)
		{
			if (!first)
			{
				obj.mtllib.materials.push_back(m);
				strcpy(m.map_Kd, "");
				strcpy(m.map_bump, "");
				
			}
			else first = false;

			skip((int&)p);
			strcpy(m.name, p);
			
			continue;
		}


		if (strcmp(word, "Kd") == 0)
		{
			vert3 v;
			sscanf(p, "%f %f %f\n", &v.x, &v.y, &v.z);
			m.Kd = v;
		}
		else if (strcmp(word, "Ka") == 0)
		{
			vert3 v;
			sscanf(p, "%f %f %f\n", &v.x, &v.y, &v.z);
			m.Ka = v;
		}
		else if (strcmp(word, "Ks") == 0)
		{
			vert3 v;
			sscanf(p, "%f %f %f\n", &v.x, &v.y, &v.z);
			m.Ks = v;
		}
		else if (strcmp(word, "Ke") == 0)
		{
			vert3 v;
			sscanf(p, "%f %f %f\n", &v.x, &v.y, &v.z);
			m.Ke = v;
		}
		else if (strcmp(word, "d") == 0)
		{
			float v;
			sscanf(p, "%f\n", &v);
			m.d = v;
		}
		else if (strcmp(word, "Ns") == 0)
		{
			float v;
			sscanf(p, "%f\n", &v);
			m.Ns= v;
		}
		else if (strcmp(word, "illum") == 0)
		{
			int v;
			sscanf(p, "%d\n", &v);
			m.illum = v;
		}
		else if (strcmp(word, "map_Kd") == 0)
		{
			skip((int&)p);
			strcpy(m.map_Kd, path);
			strcat(m.map_Kd, p);
		}
		else if (strcmp(word, "map_bump") == 0)
		{
			skip((int&)p);
			strcpy(m.map_bump, path);
			strcat(m.map_bump, p);
		}
		
	}

	if (!first)
	{
		obj.mtllib.materials.push_back(m);
	}




	return true;
}