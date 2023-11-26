#include "cube.h"
#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "glm/gtx/transform.hpp"
#include "texture.hpp"

glm::mat4 projectMat;
glm::mat4 viewMat;
glm::mat4 modelMat = glm::mat4(1.0f); 

GLuint pvmMatrixID;
GLuint projectMatrixID; 
GLuint viewMatrixID;
GLuint modelMatrixID;
GLuint textureModeID;
GLuint Texture;
GLuint TextureHead;

float rotAngle = 0.0f;
int isRotate = false;
int isTexture = false;

typedef glm::vec4  point4;
typedef glm::vec3  normal3;
typedef glm::vec2  texture2;

const int NumVertices = 36; // ť�� 6�� * �� �� �� 2���� �ﰢ�� * �ﰢ�� �� 3���� ����

point4 points[NumVertices];
normal3 normals[NumVertices]; // ���� ���� ����
texture2 texCoords[NumVertices]; // �ؽ��� ��ǥ ����

// ������ �߽����� �ϴ� ť���� ����
point4 vertices[8] = {
	point4(-0.5, -0.5, 0.5, 1.0),
	point4(-0.5, 0.5, 0.5, 1.0),
	point4(0.5, 0.5, 0.5, 1.0),
	point4(0.5, -0.5, 0.5, 1.0),
	point4(-0.5, -0.5, -0.5, 1.0),
	point4(-0.5, 0.5, -0.5, 1.0),
	point4(0.5, 0.5, -0.5, 1.0),
	point4(0.5, -0.5, -0.5, 1.0)
};

//----------------------------------------------------------------------------

// quad()�� �� �鿡 ���� �� ���� �ﰢ���� �����ϰ� ������ ������ �ο��Ѵ�.
int Index = 0;
void quad(int a, int b, int c, int d)
{
	// ť�� ���� ���� ���� ���
	normal3 u = vertices[b] - vertices[a];
	normal3 v = vertices[c] - vertices[b];
	normal3 normal = glm::normalize(glm::cross(u, v));

	// ù ��° �ﰢ��
	points[Index] = vertices[a]; normals[Index] = normal; texCoords[Index] = glm::vec2(0.0f, 1.0f); Index++;
	points[Index] = vertices[b]; normals[Index] = normal; texCoords[Index] = glm::vec2(1.0f, 1.0f); Index++;
	points[Index] = vertices[c]; normals[Index] = normal; texCoords[Index] = glm::vec2(1.0f, 0.0f); Index++;

	// �� ��° �ﰢ��
	points[Index] = vertices[a]; normals[Index] = normal; texCoords[Index] = glm::vec2(0.0f, 1.0f); Index++;
	points[Index] = vertices[c]; normals[Index] = normal; texCoords[Index] = glm::vec2(1.0f, 0.0f); Index++;
	points[Index] = vertices[d]; normals[Index] = normal; texCoords[Index] = glm::vec2(0.0f, 0.0f); Index++;
}

//----------------------------------------------------------------------------

// 12���� �ﰢ���� �����Ѵ�. �� ������ 36��
void colorcube()
{
	quad(1, 0, 3, 2);
	quad(2, 3, 7, 6);
	quad(3, 0, 4, 7);
	quad(6, 5, 1, 2);
	quad(4, 5, 6, 7);
	quad(5, 4, 0, 1);
}

//----------------------------------------------------------------------------

// OpenGL �ʱ�ȭ
void init()
{
	colorcube();

	// VAO ����
	GLuint vao;
	glGenVertexArrays(1, &vao);
	glBindVertexArray(vao);

	// ���� ��ü ���� �� �ʱ�ȭ
	GLuint buffer;
	glGenBuffers(1, &buffer);
	glBindBuffer(GL_ARRAY_BUFFER, buffer);

	// ���� �����͸� ���� �����Ϳ� ���� ���� �����ͷ� �����Ѵ�.
	glBufferData(GL_ARRAY_BUFFER, sizeof(points) + sizeof(normals) + sizeof(texCoords), NULL, GL_STATIC_DRAW);
	glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(points), points);
	glBufferSubData(GL_ARRAY_BUFFER, sizeof(points), sizeof(normals), normals);
	glBufferSubData(GL_ARRAY_BUFFER, sizeof(points) + sizeof(normals), sizeof(texCoords), texCoords);

	// ���̴��� �ε��ϰ� ���̴� ���α׷� ���
	GLuint program = InitShader("src/vshader.glsl", "src/fshader.glsl");
	glUseProgram(program);

	// ���� �迭 ����
	GLuint vPosition = glGetAttribLocation(program, "vPosition");
	glEnableVertexAttribArray(vPosition);
	glVertexAttribPointer(vPosition, 4, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(0));

	// ���� ���� ����
	GLuint vNormal = glGetAttribLocation(program, "vNormal");
	glEnableVertexAttribArray(vNormal);
	glVertexAttribPointer(vNormal, 3, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(sizeof(points)));

	// �ؽ��� ��ǥ ����
	GLuint vTexCoord = glGetAttribLocation(program, "vTexCoord");
	glEnableVertexAttribArray(vTexCoord);
	glVertexAttribPointer(vTexCoord, 2, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(sizeof(points) + sizeof(normals)));

	projectMatrixID = glGetUniformLocation(program, "mProject");
	projectMat = glm::perspective(glm::radians(65.0f), 1.0f, 0.1f, 100.0f);
	glUniformMatrix4fv(projectMatrixID, 1, GL_FALSE, &projectMat[0][0]);

	viewMatrixID = glGetUniformLocation(program, "mView");
	viewMat = glm::lookAt(glm::vec3(0, 0, 2), glm::vec3(0, 0, 0), glm::vec3(0, 1, 0));
	glUniformMatrix4fv(viewMatrixID, 1, GL_FALSE, &viewMat[0][0]);

	modelMatrixID = glGetUniformLocation(program, "mModel");
	modelMat = glm::mat4(1.0f);
	glUniformMatrix4fv(modelMatrixID, 1, GL_FALSE, &modelMat[0][0]);

	pvmMatrixID = glGetUniformLocation(program, "mPVM");
	glm::mat4 pvmMat = projectMat * viewMat * modelMat;
	glUniformMatrix4fv(pvmMatrixID, 1, GL_FALSE, &pvmMat[0][0]);

	textureModeID = glGetUniformLocation(program, "isTexture");
	glUniform1i(textureModeID, isTexture);

	// �ؽ��ĸ� �ҷ��´�.
	Texture = loadBMP_custom("cat.bmp");
	TextureHead = loadBMP_custom("box.bmp");

	// "myTextureSampler" �������� ���� �ڵ��� ��´�.
	GLuint TextureID = glGetUniformLocation(program, "cubeTexture");

	// �ؽ�ó ���� 0�� �ؽ�ó�� ���ε�
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, Texture);

	// Texture Unit 0�� ����ϵ��� "myTextureSampler" ���÷��� ����
	glUniform1i(TextureID, 0);

	glEnable(GL_DEPTH_TEST);
	glClearColor(0.0, 0.0, 0.0, 1.0);
}

//----------------------------------------------------------------------------
void drawAnimal(glm::mat4 catMat)
{
	float time = glutGet(GLUT_ELAPSED_TIME) / 1000.0f; // ���� �ð��� �� ������ ������.
	float legRotation = sin(time * 5.0f) * 0.5f; // �ٸ��� ȸ�������� ���
	float tailRotation = sin(time * 25.0f) * 0.3f; // ������ ȸ�������� ���

	glm::mat4 modelMat, pvmMat;
	glm::vec3 LFPos[2];
	glm::vec3 LRPos[2];
	glm::vec3 RFPos[2];
	glm::vec3 RRPos[2];
	glm::vec3 earPos[2];

	LFPos[0] = glm::vec3(0.25, -0.25, -0.125);  // ���� ���� �����
	LFPos[1] = glm::vec3(0.25, -0.35, -0.125);  // ���� ���� ���Ƹ�
	LRPos[0] = glm::vec3(-0.25, -0.25, -0.125);   // ���� �ĸ� �����
	LRPos[1] = glm::vec3(-0.25, -0.35, -0.125);	// ���� �ĸ� ���Ƹ�

	RFPos[0] = glm::vec3(0.25, -0.25, 0.125); // ���� ���� �����
	RFPos[1] = glm::vec3(0.25, -0.35, 0.125); // ���� ���� ���Ƹ�
	RRPos[0] = glm::vec3(-0.25, -0.25, 0.125);  // ���� �ĸ� �����
	RRPos[1] = glm::vec3(-0.25, -0.35, 0.125);  // ���� �ĸ� ���Ƹ�

	earPos[0] = glm::vec3(0.5, 0.35, 0.1); // ���� ��
	earPos[1] = glm::vec3(0.5, 0.35, -0.1); // ������ ��

	// �� 
	modelMat = glm::scale(catMat, glm::vec3(0.6, 0.4, 0.4));
	pvmMat = projectMat * viewMat * modelMat;
	glUniformMatrix4fv(pvmMatrixID, 1, GL_FALSE, &pvmMat[0][0]);
	glDrawArrays(GL_TRIANGLES, 0, NumVertices);

	// �Ӹ��� �ڽ� �ؽ��� ���ε�
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, TextureHead);

	// �Ӹ�
	modelMat = glm::translate(catMat, glm::vec3(0.425, 0.2, 0));
	modelMat = glm::scale(modelMat, glm::vec3(0.25, 0.25, 0.25));
	pvmMat = projectMat * viewMat * modelMat;
	glUniformMatrix4fv(pvmMatrixID, 1, GL_FALSE, &pvmMat[0][0]);
	glDrawArrays(GL_TRIANGLES, 0, NumVertices);

	// �Ӹ� �� �ٸ� �κ��� ����� �ؽ�ó�� �缳��
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, Texture);

	// ����
	glm::vec3 tailBasePos = glm::vec3(-0.3, 0, 0);  // ������ �޺κп� ��ġ
	modelMat = glm::translate(catMat, tailBasePos);  // ������ �޺κ� ��ġ�� �̵�
	modelMat = glm::rotate(modelMat, tailRotation, glm::vec3(0, 1, 0));  // ȸ�� ����
	modelMat = glm::translate(modelMat, -tailBasePos + glm::vec3(-0.4, 0, 0));  // ���� ������ ��ġ�� �ǵ�����
	modelMat = glm::scale(modelMat, glm::vec3(0.2, 0.1, 0.1));
	pvmMat = projectMat * viewMat * modelMat;
	glUniformMatrix4fv(pvmMatrixID, 1, GL_FALSE, &pvmMat[0][0]);
	glDrawArrays(GL_TRIANGLES, 0, NumVertices);

	// ���� ����� �ٸ� (LF)
	for (int i = 0; i < 2; i++)
	{
		glm::vec3 legRotationAxis = glm::vec3(0.25, -0.2, 0); // ������ �Ʒ��κ� ��ġ
		modelMat = glm::translate(catMat, legRotationAxis);  // �ٸ��� ��� ��ġ�� �̵�
		modelMat = glm::rotate(modelMat, legRotation, glm::vec3(0, 0, 0.5));  // ȸ�� ����
		modelMat = glm::translate(modelMat, -legRotationAxis + LFPos[i]);  // ������ ��ġ�� �ǵ�����
		modelMat = glm::scale(modelMat, glm::vec3(0.1, 0.1, 0.1));
		pvmMat = projectMat * viewMat * modelMat;
		glUniformMatrix4fv(pvmMatrixID, 1, GL_FALSE, &pvmMat[0][0]);
		glDrawArrays(GL_TRIANGLES, 0, NumVertices);
	}

	// ���� �ĸ�� �ٸ� (LR)
	for (int i = 0; i < 2; i++)
	{
		glm::vec3 legRotationAxis = glm::vec3(-0.25, -0.2, 0);
		modelMat = glm::translate(catMat, legRotationAxis);
		modelMat = glm::rotate(modelMat, legRotation, glm::vec3(0, 0, 0.5));
		modelMat = glm::translate(modelMat, -legRotationAxis + LRPos[i]);
		modelMat = glm::scale(modelMat, glm::vec3(0.1, 0.1, 0.1));
		pvmMat = projectMat * viewMat * modelMat;
		glUniformMatrix4fv(pvmMatrixID, 1, GL_FALSE, &pvmMat[0][0]);
		glDrawArrays(GL_TRIANGLES, 0, NumVertices);
	}

	// ���� ����� �ٸ� (RF)
	for (int i = 0; i < 2; i++)
	{
		glm::vec3 legRotationAxis = glm::vec3(0.25, -0.2, 0);
		modelMat = glm::translate(catMat, legRotationAxis);
		modelMat = glm::rotate(modelMat, -legRotation, glm::vec3(0, 0, 0.5));
		modelMat = glm::translate(modelMat, -legRotationAxis + RFPos[i]);
		modelMat = glm::scale(modelMat, glm::vec3(0.1, 0.1, 0.1));
		pvmMat = projectMat * viewMat * modelMat;
		glUniformMatrix4fv(pvmMatrixID, 1, GL_FALSE, &pvmMat[0][0]);
		glDrawArrays(GL_TRIANGLES, 0, NumVertices);
	}

	// ���� �ĸ�� �ٸ� (RR)
	for (int i = 0; i < 2; i++)
	{
		glm::vec3 legRotationAxis = glm::vec3(-0.25, -0.2, 0);
		modelMat = glm::translate(catMat, legRotationAxis);
		modelMat = glm::rotate(modelMat, -legRotation, glm::vec3(0, 0, 0.5));
		modelMat = glm::translate(modelMat, -legRotationAxis + RRPos[i]);
		modelMat = glm::scale(modelMat, glm::vec3(0.1, 0.1, 0.1));
		pvmMat = projectMat * viewMat * modelMat;
		glUniformMatrix4fv(pvmMatrixID, 1, GL_FALSE, &pvmMat[0][0]);
		glDrawArrays(GL_TRIANGLES, 0, NumVertices);
	}

	// �Ӹ��� �ڽ��� �Ϳ��� ����
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, TextureHead);

	// ��
	for (int i = 0; i < 2; i++)
	{
		modelMat = glm::translate(catMat, earPos[i]);
		modelMat = glm::scale(modelMat, glm::vec3(0.05, 0.05, 0.05));
		pvmMat = projectMat * viewMat * modelMat;
		glUniformMatrix4fv(pvmMatrixID, 1, GL_FALSE, &pvmMat[0][0]);
		glDrawArrays(GL_TRIANGLES, 0, NumVertices);
	}

	// �� �� �ٸ� �κ��� ����� �ؽ�ó�� �缳��
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, Texture);
}

void display(void)
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	drawAnimal(modelMat);
	glUniformMatrix4fv(modelMatrixID, 1, GL_FALSE, &modelMat[0][0]);

	glutSwapBuffers();
}

//----------------------------------------------------------------------------

void idle()
{
	static int prevTime = glutGet(GLUT_ELAPSED_TIME);
	int currTime = glutGet(GLUT_ELAPSED_TIME);

	if (abs(currTime - prevTime) >= 20)
	{
		float t = abs(currTime - prevTime);
		float speed = 720.0f / 10000.0f;
		modelMat = glm::rotate(modelMat, glm::radians(t * speed), glm::vec3(1.0f, 1.0f, 0.0f));
		prevTime = currTime;
		glutPostRedisplay();
	}
}

//----------------------------------------------------------------------------

void
keyboard(unsigned char key, int x, int y)
{
	switch (key) {
	case 't': case 'T':
		isTexture = !isTexture;
		glUniform1i(textureModeID, isTexture);
		glutPostRedisplay();
		break;
	case 'r': case 'R':
		isRotate = !isRotate;
		glutPostRedisplay();
		break;
	case 033:  // Escape key
	case 'q': case 'Q':
		exit(EXIT_SUCCESS);
		break;
	}
}

//----------------------------------------------------------------------------

void resize(int w, int h)
{
	float ratio = (float)w / (float)h;
	glViewport(0, 0, w, h);

	projectMat = glm::perspective(glm::radians(65.0f), ratio, 0.1f, 100.0f);
	glUniformMatrix4fv(projectMatrixID, 1, GL_FALSE, &projectMat[0][0]);
	glutPostRedisplay();
}

//----------------------------------------------------------------------------

int
main(int argc, char** argv)
{
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_RGBA | GLUT_DOUBLE | GLUT_DEPTH);
	glutInitWindowSize(512, 512);
	glutInitContextVersion(3, 2);
	glutInitContextProfile(GLUT_CORE_PROFILE);
	glutCreateWindow("Color Cat");

	glewInit();

	init();

	glutDisplayFunc(display);
	glutKeyboardFunc(keyboard);
	glutReshapeFunc(resize);
	glutIdleFunc(idle);

	glutMainLoop();
	return 0;
}