#include <GL/glew.h>
#include "core/GXCommon.hpp"
#include "core/CModelData.hpp"
#include "core/CMesh.hpp"
#include "ui/CGLViewer.hpp"

#include <Athena/MemoryReader.hpp>
#include <QFileInfo>
#include <QDir>
#include <cmath>
#include <GL/gl.h>
#include <chrono>
#include <ctime>

#ifndef M_PI
#define M_PI    3.14159265358979323846
#endif


void drawWireSphere(float radius, atUint32 longs, atUint32 lats)
{
    for (int i = 1; i <= lats; i++)
    {
        float lat0 = M_PI * (-0.5f + (float)(i - 1) / (float)lats);
        float z0  = radius * sinf(lat0);
        float zr0 =  radius * cosf(lat0);

        float lat1 = M_PI * (-0.5f + (float)i / (float)lats);
        float z1 = radius * sinf(lat1);
        float zr1 = radius * cosf(lat1);

        glBegin(GL_QUAD_STRIP);
        for (int j = 0; j <= longs; j++)
        {
            float lng = 2 * M_PI * (float)(j - 1) / (float)longs;
            float x = cosf(lng);
            float y = sinf(lng);
            glNormal3f(x * zr1, y * zr1, z1);
            glVertex3f(x * zr1, y * zr1, z1);
            glNormal3f(x * zr0, y * zr0, z0);
            glVertex3f(x * zr0, y * zr0, z0);
        }
        glEnd();
    }
}

void drawOutlinedCube(const glm::vec3& position, const glm::vec4& mainColor, const glm::vec4& outlineColor, const glm::vec3& scale)
{
    static float cubeCoords[] =
    {
        -1.f, -1.f,  1.f,
        -1.f, -1.f, -1.f,
         1.f, -1.f, -1.f,
         1.f, -1.f,  1.f,
        -1.f,  1.f,  1.f,
        -1.f,  1.f, -1.f,
         1.f,  1.f, -1.f,
         1.f,  1.f,  1.f
    };

    static GLuint cubeIndices[6*4] = {
        4, 5, 1, 0,
        5, 6, 2, 1,
        6, 7, 3, 2,
        7, 4, 0, 3,
        0, 1, 2, 3,
        7, 6, 5, 4
    };

    static GLuint bufferObjs[2] = {0, 0};
    if (bufferObjs[0] == 0)
    {
        glGenBuffers(2, bufferObjs);
        glBindBuffer(GL_ARRAY_BUFFER, bufferObjs[0]);
        glBufferData(GL_ARRAY_BUFFER, sizeof(cubeCoords), cubeCoords, GL_STATIC_DRAW);

        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, bufferObjs[1]);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(cubeIndices), cubeIndices, GL_STATIC_DRAW);
    }

    glEnableVertexAttribArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, bufferObjs[0]);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, bufferObjs[1]);
    glDrawElements(GL_QUADS, sizeof(cubeIndices), GL_UNSIGNED_INT, (void*)0);

    glDisableVertexAttribArray(0);
}


void drawSolidSphere(double radius, atUint32 slices, atUint32 stacks)
{
    atUint32 i, j;
    for(i = 0; i <= slices; i++)
    {
        double lat0 = M_PI * (-0.5 + (double) (i - 1) / slices);
        double z0  = sin(lat0);
        double zr0 =  cos(lat0);

        double lat1 = M_PI * (-0.5 + (double) i / slices);
        double z1 = sin(lat1);
        double zr1 = cos(lat1);

        glBegin(GL_QUAD_STRIP);
        for(j = 0; j <= stacks; j++) {
            double lng = 2 * M_PI * (double) (j - 1) / stacks;
            double x = cos(lng);
            double y = sin(lng);
            glNormal3f(x * zr1 * radius, y * zr1 * radius, z1 * radius);
            glVertex3f(x * zr1 * radius, y * zr1 * radius, z1 * radius);
            glNormal3f(x * zr0 * radius, y * zr0 * radius, z0 * radius);
            glVertex3f(x * zr0 * radius, y * zr0 * radius, z0 * radius);
        }
        glEnd();
    }
}

void drawTexturedCube(atUint32 texture, const glm::vec3& position, float alpha, const glm::vec3& scale)
{
    static float cubeCoords[8][3] =
    {
        {-1.f, -1.f,  1.f},
        {-1.f, -1.f, -1.f},
        { 1.f, -1.f, -1.f},
        { 1.f, -1.f,  1.f},
        {-1.f,  1.f,  1.f},
        {-1.f,  1.f, -1.f},
        { 1.f,  1.f, -1.f},
        { 1.f,  1.f,  1.f}
    };
    static float texCoords[4][2] =
    {
        {1.f, 1.f},
        {0.f, 1.f},
        {0.f, 0.f},
        {1.f, 0.f}
    };

    glColor4f(1, 1, 1, alpha);
    glPushAttrib(GL_ENABLE_BIT);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glAlphaFunc(GL_GEQUAL, 0.05f);
    glEnable(GL_ALPHA_TEST);
    glPushMatrix();
    glTranslatef(position.x, position.y, position.z);
    glScalef(scale.y, scale.x, scale.z);
    glActiveTexture(GL_TEXTURE0);
    glEnable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, texture);
    glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);

    glBegin(GL_QUADS);
    {
        // top
        glTexCoord2fv(texCoords[0]);
        glVertex3fv(cubeCoords[4]);
        glTexCoord2fv(texCoords[1]);
        glVertex3fv(cubeCoords[5]);
        glTexCoord2fv(texCoords[2]);
        glVertex3fv(cubeCoords[1]);
        glTexCoord2fv(texCoords[3]);
        glVertex3fv(cubeCoords[0]);
        // front
        glTexCoord2fv(texCoords[0]);
        glVertex3fv(cubeCoords[5]);
        glTexCoord2fv(texCoords[1]);
        glVertex3fv(cubeCoords[6]);
        glTexCoord2fv(texCoords[2]);
        glVertex3fv(cubeCoords[2]);
        glTexCoord2fv(texCoords[3]);
        glVertex3fv(cubeCoords[1]);
        // right
        glTexCoord2fv(texCoords[0]);
        glVertex3fv(cubeCoords[6]);
        glTexCoord2fv(texCoords[1]);
        glVertex3fv(cubeCoords[7]);
        glTexCoord2fv(texCoords[2]);
        glVertex3fv(cubeCoords[3]);
        glTexCoord2fv(texCoords[3]);
        glVertex3fv(cubeCoords[2]);
        // left
        glTexCoord2fv(texCoords[0]);
        glVertex3fv(cubeCoords[7]);
        glTexCoord2fv(texCoords[1]);
        glVertex3fv(cubeCoords[4]);
        glTexCoord2fv(texCoords[2]);
        glVertex3fv(cubeCoords[0]);
        glTexCoord2fv(texCoords[3]);
        glVertex3fv(cubeCoords[3]);
        // bottom
        glTexCoord2fv(texCoords[0]);
        glVertex3fv(cubeCoords[0]);
        glTexCoord2fv(texCoords[1]);
        glVertex3fv(cubeCoords[1]);
        glTexCoord2fv(texCoords[2]);
        glVertex3fv(cubeCoords[2]);
        glTexCoord2fv(texCoords[3]);
        glVertex3fv(cubeCoords[3]);
        // back
        glTexCoord2fv(texCoords[0]);
        glVertex3fv(cubeCoords[7]);
        glTexCoord2fv(texCoords[1]);
        glVertex3fv(cubeCoords[6]);
        glTexCoord2fv(texCoords[2]);
        glVertex3fv(cubeCoords[5]);
        glTexCoord2fv(texCoords[3]);
        glVertex3fv(cubeCoords[4]);
    }
    glEnd();
    glPopMatrix();
    glPopAttrib();
}


void GXSetBlendMode(EBlendMode src, EBlendMode dst)
{
    atUint32 glSrc = GL_ONE;
    switch(src)
    {
        case EBlendMode::Zero: glSrc = GL_ZERO; break;
        case EBlendMode::One: glSrc = GL_ONE; break;
        case EBlendMode::SrcClr: glSrc = GL_SRC_COLOR; break;
        case EBlendMode::InvSrcClr: glSrc = GL_ONE_MINUS_SRC_COLOR; break;
        case EBlendMode::SrcAlpha: glSrc = GL_SRC_ALPHA; break;
        case EBlendMode::InvSrcAlpha: glSrc = GL_ONE_MINUS_SRC_ALPHA; break;
        case EBlendMode::DstAlpha: glSrc = GL_DST_ALPHA; break;
        case EBlendMode::InvDstAlpha: glSrc = GL_ONE_MINUS_DST_ALPHA; break;
        default:
            break;
    }

    atUint32 glDst = GL_ONE;
    switch(dst)
    {
        case EBlendMode::Zero: glDst = GL_ZERO; break;
        case EBlendMode::One: glDst = GL_ONE; break;
        case EBlendMode::DstClr: glDst = GL_SRC_COLOR; break;
        case EBlendMode::InvDstClr: glDst = GL_ONE_MINUS_SRC_COLOR; break;
        case EBlendMode::SrcAlpha: glDst = GL_SRC_ALPHA; break;
        case EBlendMode::InvSrcAlpha: glDst = GL_ONE_MINUS_SRC_ALPHA; break;
        case EBlendMode::DstAlpha: glDst = GL_DST_ALPHA; break;
        case EBlendMode::InvDstAlpha: glDst = GL_ONE_MINUS_DST_ALPHA; break;
        default:
            break;
    }
    glBlendFunc(glSrc, glDst);
}


atUint64 assetIdFromPath(const std::string& filepath)
{
    QString baseName = QFileInfo(QString::fromStdString(filepath)).baseName();

    if (baseName.at(1) == '_')
        baseName = baseName.remove(0, 2);

    bool ok = false;
    atUint64 id = QString("0x" + baseName).toULongLong(&ok, 16);

    if (ok)
        return id;

    return (atUint64)(-1);
}

void drawBoundingBox(SBoundingBox bbox)
{
    glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
    glDisable(GL_LIGHTING);
    glDisable(GL_CULL_FACE);
    glColor3f(255.0f, 0.0f, 0.0f);
    glBegin(GL_QUADS);
    {
        // Back
        glVertex3f(bbox.min.x, bbox.min.y, bbox.min.z);
        glVertex3f(bbox.max.x, bbox.min.y, bbox.min.z);
        glVertex3f(bbox.max.x, bbox.max.y, bbox.min.z);
        glVertex3f(bbox.min.x, bbox.max.y, bbox.min.z);

        // Front
        glVertex3f(bbox.min.x, bbox.min.y, bbox.max.z);
        glVertex3f(bbox.max.x, bbox.min.y, bbox.max.z);
        glVertex3f(bbox.max.x, bbox.max.y, bbox.max.z);
        glVertex3f(bbox.min.x, bbox.max.y, bbox.max.z);

        // Right
        glVertex3f(bbox.max.x, bbox.max.y, bbox.max.z);
        glVertex3f(bbox.max.x, bbox.min.y, bbox.max.z);
        glVertex3f(bbox.max.x, bbox.min.y, bbox.min.z);
        glVertex3f(bbox.max.x, bbox.max.y, bbox.min.z);

        // Left
        glVertex3f(bbox.min.x, bbox.max.y, bbox.max.z);
        glVertex3f(bbox.min.x, bbox.min.y, bbox.max.z);
        glVertex3f(bbox.min.x, bbox.min.y, bbox.min.z);
        glVertex3f(bbox.min.x, bbox.max.y, bbox.min.z);
    }

    glEnd();
    glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
}

void drawCone(float width, float height, float offset)
{
    glBegin(GL_TRIANGLE_FAN);
    glVertex3f(0, 0, offset + height);
    float x, y;
    for (float rads = 0.0f; rads < M_PI * 2.0f; rads += 0.1f)
    {
        x = cosf(rads) * width;
        y = sinf(rads) * width;
        glVertex3f(x, y, offset);
    }
    x = cosf(0.0f) * width;
    y = sinf(0.0f) * width;
    glVertex3f(x, y, offset);
    glEnd();
}


void drawAxis(glm::vec3 translation, glm::vec3 orientation, float scale, bool disableDepth)
{
    if (disableDepth)
        glDisable(GL_DEPTH_TEST);
    glPushMatrix();
    glTranslatef(translation.x, translation.y, translation.z);
    // Rotate in reverse order, because lawl trig
    glRotatef(orientation.z, 0, 0, 1);
    glRotatef(orientation.y, 0, 1, 0);
    glRotatef(orientation.x, 1, 0, 0);
    glScalef(scale, scale, scale);
    glBegin(GL_LINES);
    glColor3ub(255,   0,   0);
    glVertex3f(  0,   0,   0);
    glColor3ub(255,   0,   0);
    glVertex3f(  1,   0,   0);
    glColor3ub(  0, 255,   0);
    glVertex3f(  0,   0,   0);
    glColor3ub(  0, 255,   0);
    glVertex3f(  0,   1,   0);
    glColor3ub(  0,   0, 255);
    glVertex3f(  0,   0,   0);
    glColor3ub(  0,   0, 255);
    glVertex3f(  0,   0,   1);
    glEnd();
    glPopMatrix();
    if (disableDepth)
        glEnable(GL_DEPTH_TEST);
}

atUint16 readAttribute(atUint16& value, atUint32 attributes, atUint32 index, Athena::io::IStreamReader& reader);
void readPrimitives(CMesh& mesh, const CMaterial& material, atUint16 dataSize, Athena::io::IStreamReader& reader)
{
    atUint32 vertexAttributes = material.vertexAttributes();

    atUint32 mainAttributes = 0;
    atUint32 subAttributes = 0;

    mainAttributes = vertexAttributes & 0x00FFFFFF;
    subAttributes = (vertexAttributes >> 0x18) & 0xFF;

    atUint32 readBytes = 0;

    while (!reader.atEnd())
    {
        atUint8 primitiveFlags = reader.readUByte();

        atUint32 indexCount = 0;

        if (primitiveFlags == 0)
        {
            break;
        }

        indexCount = reader.readUint16();
        readBytes += 2;
        if (indexCount == 0 || reader.atEnd())
        {
            break;
        }

        CPrimitive primitive;
        primitive.primitive = (EPrimitive)(primitiveFlags & 0xF8);
        primitive.vertexFormatIdx = primitiveFlags & 0x07;

        atUint32 startReadBytes = readBytes;
        atUint32 currentPos = reader.position();

        bool valid = false;
        primitive.indices.resize(indexCount);
        SVertexDescriptor* pFaces = &primitive.indices.front();

        if (mainAttributes)
        {
            reader.seek(currentPos, Athena::SeekOrigin::Begin);
            readBytes = startReadBytes;

            atUint32 attributes = 0;
            for (atUint32 v = 0; v < indexCount; ++v)
            {
                // read unknown attribute indices
                attributes = subAttributes;
                while (attributes)
                {
                    if (attributes & 1)
                    {
                        reader.readUByte();
                        readBytes += 1;
                    }
                    attributes >>= 1;
                }

                // read main attribute indices
                attributes = mainAttributes;

                readBytes += readAttribute(pFaces[v].position, attributes, 0, reader);
                readBytes += readAttribute(pFaces[v].normal, attributes, 1, reader);

                for (int iColor = 0; iColor < 2; ++iColor)
                {
                    readBytes += readAttribute(pFaces[v].clr[iColor], attributes, iColor + 2, reader);
                }

                for (int iUV = 0; iUV < 8; ++iUV)
                {
                    readBytes += readAttribute(pFaces[v].texCoord[iUV], attributes, iUV + 4, reader);
                }

                for (int iUnk = 0; iUnk < 4; ++iUnk)
                {
                    readBytes += readAttribute(pFaces[v].unkIndex[iUnk], attributes, iUnk + 12, reader);
                }
            }

            valid = true;
        }


        if (!valid)
        {
            primitive.primitive = (EPrimitive)0;
            break;
        }
        mesh.m_primitives.push_back(primitive);
    }
}


atUint16 readAttribute(atUint16 & value, atUint32 attributes, atUint32 index, Athena::io::IStreamReader& reader)
{
    atUint16 readCount = 0;

    switch ((attributes >> (index << 1)) & 3)
    {
        case 3:
            value = reader.readUint16();
            readCount = 2;
            break;
        case 2:
            value = reader.readUByte();
            readCount = 1;
            break;
        default:
            break;
    }

    return readCount;
}

typedef std::chrono::high_resolution_clock hrClock_t;
typedef std::chrono::duration<float> fsec;
typedef std::chrono::duration<long, std::milli> millisec;
static auto start = hrClock_t::now();

long hiresTimeMS()
{
    auto now = hrClock_t::now();

    return std::chrono::duration_cast<std::chrono::milliseconds>(now - start).count();
}

float hiresTimeSec()
{
    auto now = hrClock_t::now();

    return fsec(now - start).count();
}

float secondsMod900()
{
    return fmod(hiresTimeSec(), 900.f);
}
