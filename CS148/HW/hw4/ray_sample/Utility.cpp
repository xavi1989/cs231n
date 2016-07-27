#include <Windows.h>
#include "Utility.h"
#include "Group.h"
#include "Surface.h"
#include "Triangle.h"

using namespace std;

bool fUseFastShading = false;
float bright_enough = 0.1f;

static float hemisphereStepLength = static_cast<float>(M_PI) / HEMISPHERE_SAMPLES;
static float eclipticStepLength = static_cast<float>(2 * M_PI) / ECLIPTIC_SAMPLES;

// Return a random float between 0.0 and 1.0.
float _rand() {
    return static_cast<float>((float)rand() / (float)RAND_MAX);
}

// RGBColor returned must have 0<=r<=1, 0<=g<=1, 0<=b<=1.
RGBColor Ray::traceForColor(const Surface& surface, int depth, int diffuse_depth) const {

    __declspec(align(16)) float t;
    Surface *s = nullptr;
	__declspec(align(16)) Vector3f normal;
    depth++;
    
    if (surface.Hit(*this, RAY_T0, RAY_T1, &t, &s, &normal) == false) {
        // Did not hit anything, return background color: black.
        return RGBColor(0.0f, 0.0f, 0.0f);
    }

    if (s == nullptr || s->GetMaterial() == nullptr) {
        // If reached here, somehow the material of this surface does not exist.
        // This should not happen.
        assert(0 && "Undefined surface or material\n");
        return RGBColor(0.0f, 0.0f, 0.0f);
    }

    __declspec(align(16)) Vector3f rayEnd = direction * t;
    __declspec(align(16)) Point3f hitPoint = Point3f(origin.x + rayEnd.x, origin.y + rayEnd.y, origin.z + rayEnd.z);
    normal.Normalize();
    // Correct normal's direction: make sure it's always on the same side as the incoming ray.
    bool fRayNormalOnSameSide = dot(normal, direction) < 0;
    normal = fRayNormalOnSameSide ? normal : normal * -1;

    std::shared_ptr<Material> pMaterial = s->GetMaterial();

    __declspec(align(16)) RGBColor materialColor = pMaterial->materialColor;
    __declspec(align(16)) RGBColor emissionColor = pMaterial->emissionColor;

    // FIXME: DEBUG DIFFUSE
#ifdef DEBUG_DIFFUSE
    assert(pMaterial->reflType != Type::SPEC);
    assert(pMaterial->reflType != Type::REFR);
#endif 

    // cut tracing depth to save time
    if (depth > 4) {
        if (pMaterial->reflType == Type::LIGHT) {
            if (fRayNormalOnSameSide) {
                return emissionColor;
            } else {
                return RGBColor(0.0f, 0.0f, 0.0f);
            }
        }
        else {
            return RGBColor(0.0f, 0.0f, 0.0f);
        }
    }

    if (pMaterial->reflType == Type::LIGHT) {
        if (fRayNormalOnSameSide) {
            return emissionColor;
        } else {
            return RGBColor(0.0f, 0.0f, 0.0f);
        }
    }
    else if (pMaterial->reflType == Type::DIFF)
    {
        diffuse_depth++;

        __declspec(align(16)) RGBColor result = RGBColor(0.0f, 0.0f, 0.0f);
        __declspec(align(16)) RGBColor brightLight = RGBColor(0.0f, 0.0f, 0.0f);

        if (!fUseFastShading) {

            // Create multiple diffuse rays bouncing off from the hit point.
            // Create orthonormal coordinate frame at the hit point (w, u, v).
            __declspec(align(16)) Vector3f w = normal;
            __declspec(align(16)) Vector3f u = cross((fabs(w.x) > 0.1) ? Vector3f(0, 1.f, 0) : Vector3f(1.f, 0, 0), w); // u is perpendicular to w
            __declspec(align(16)) Vector3f v = cross(w, u); // z is perpendicular to w and u
            u.Normalize();
            v.Normalize();

            uint8_t longitude_coord_start = rand() % HEMISPHERE_SAMPLES;
            uint8_t latitude_coord_start = rand() % ECLIPTIC_SAMPLES;

            for (uint8_t longitude_coord_i = 0; longitude_coord_i < HEMISPHERE_SAMPLES; longitude_coord_i++)
            {
                for (uint8_t latitude_coord_i = 0; latitude_coord_i < ECLIPTIC_SAMPLES; latitude_coord_i++)
                {
                    uint8_t longitude_coord = (longitude_coord_i + HEMISPHERE_SAMPLES) % HEMISPHERE_SAMPLES;
                    uint8_t latitude_coord = (latitude_coord_i + ECLIPTIC_SAMPLES) % ECLIPTIC_SAMPLES;

                    float theta = hemisphereStepLength * (HEMISPHERE_SAMPLES / 2 + ((longitude_coord % 2) ? -1 : 1) * (longitude_coord + 1) / 2 + _rand());
                    float phi = eclipticStepLength * (latitude_coord + _rand());

                    __declspec(align(16)) Vector3f diffRelfDir_u = u * (sin(theta) * cos(phi)); 
                    __declspec(align(16)) Vector3f diffRelfDir_v = v * (sin(theta) * sin(phi));
                    __declspec(align(16)) Vector3f diffRelfDir_w = w * cos(theta); 
                    __declspec(align(16)) Vector3f diffRelfDir_1 = diffRelfDir_u + diffRelfDir_v;
                    __declspec(align(16)) Vector3f diffRelfDir = diffRelfDir_1 + diffRelfDir_w;

                    Ray diffRelfRay(hitPoint, diffRelfDir, refrIndex);
                    __declspec(align(16)) RGBColor lightColor = diffRelfRay.traceForColor(surface, depth, diffuse_depth);

                    if (diffuse_depth == 1)
                    {
                        if (lightColor > brightLight) {
                            brightLight = lightColor;
                        }
                    }
                    else if (diffuse_depth >= 2)
                    {
                        // Performance Optimization
                        if (lightColor.r >= bright_enough || lightColor.g >= bright_enough || lightColor.b >= bright_enough)
                        {
                            __declspec(align(16)) RGBColor diffColor = lightColor * materialColor;
                            return diffColor;
                        }

                        if (lightColor > brightLight) {
                            brightLight = lightColor;
                        }

                        // Russian Roulette
                        if ((rand() & ((1 << diffuse_depth) - 1)) > 4) {
                            goto RETURN_DIFF_LIGHT;
                        }
                    }
                }
            }

RETURN_DIFF_LIGHT:
            // printf("brightLight %f %f %f\n", brightLight.r, brightLight.g, brightLight.b);
            __declspec(align(16)) RGBColor diffColor = brightLight * materialColor;
            return diffColor;

        } 
        else // (fUseFastShading == true)
        {
            std::vector<const Surface*> lights;
            surface.GatherLightSources(lights);
            std::vector<const Surface*>::const_iterator it;
            for (it = lights.begin(); it != lights.end(); ++it)
            {
                const Surface *light = *it;
                __declspec(align(16)) RGBColor lightResult;

                for (int gridIndex = 0; gridIndex < LIGHT_SAMPLES; gridIndex++)
                {
                    __declspec(align(16)) Vector3f L(hitPoint /*Start*/, light->GetLightPointInGrid(gridIndex) /*End*/);
                    L.Normalize();
                    Ray rayTowardsLight(hitPoint, L);
                    __declspec(align(16)) float dotP = dot(L, normal);
                    __declspec(align(16)) RGBColor DiffC = (dotP > 0) ? rayTowardsLight.traceForLight(surface, light) * dotP * s->GetMaterial()->diffAmount * materialColor : RGBColor();

                    lightResult = lightResult + DiffC;
                }

                lightResult = (lightResult * (1.0f / LIGHT_SAMPLES)).Trunc();
                result = result + lightResult;
            }
            return result.Trunc();
        }
        assert(0 && "DIFF Color Not Return\n");
    }
    else if (pMaterial->reflType == Type::SPEC)
    {
        // Create reflection ray.
        __declspec(align(16)) float reflDot  = dot(direction, normal);
        reflDot = reflDot * 2.0f;
        __declspec(align(16)) Vector3f reflDir_tmp = normal * reflDot;
        __declspec(align(16)) Vector3f reflDir = direction - reflDir_tmp;
        __declspec(align(16)) Ray reflRay = Ray(hitPoint, reflDir, refrIndex);

        // __declspec(align(16)) RGBColor result = emissionColor + materialColor * reflRay.traceForColor(surface, depth, diffuse_depth);

        __declspec(align(16)) RGBColor result = reflRay.traceForColor(surface, depth, diffuse_depth);
        result = result * materialColor;
        result = result + emissionColor;
        
        result.Trunc();
        return result;
    }
    else if (pMaterial->reflType == Type::REFR)
    {
        __declspec(align(16)) float reflDot = dot(direction, normal);
        reflDot = reflDot * 2.0f;
        __declspec(align(16)) Vector3f reflDir_tmp = normal * reflDot;
        __declspec(align(16)) Vector3f reflDir = direction - reflDir_tmp;
        __declspec(align(16)) Ray reflRay = Ray(hitPoint, reflDir, refrIndex);

        bool fOutSideIn = fRayNormalOnSameSide;

        // Ideal Dielectric Refraction
        float ni = refrIndex;
        float nt = (fOutSideIn) ? pMaterial->refrIndex : pMaterial->extRefrIndex;
        float nnt = ni / nt;         // sin(t) / sin(i)

        __declspec(align(16)) float cosi = -dot(direction, normal);            // cos(i)
        float cos2t = 1 - nnt * nnt * (1 - cosi * cosi); // 1 - (sin(t) / sin(i))^2 * (1 - cos(i)^2)
        if (cos2t < 0) // Total internal Reflection
            return emissionColor + materialColor * reflRay.traceForColor(surface, depth, diffuse_depth);

        float cost = sqrt(cos2t);

        // Refraction direction ref:
        // http://graphics.stanford.edu/courses/cs148-10-summer/docs/2006--degreve--reflection_refraction.pdf
        // formula (22)
        __declspec(align(16)) Vector3f tDir_1 = direction * nnt;
        __declspec(align(16)) Vector3f tDir_2 = normal * (nnt * cosi - cost);
        __declspec(align(16)) Vector3f tDir = tDir_1 + tDir_2;
        __declspec(align(16)) Ray refracRay(hitPoint, tDir, nt);

        // Reflection intensity ref:
        // https://en.wikipedia.org/wiki/Fresnel_equations
        float Rs = pow((nnt * cosi - cost) / (nnt * cosi + cost), 2);
        float Rp = pow((nnt * cost - cosi) / (nnt * cost + cosi), 2);
        __declspec(align(16)) float refl_Intensity = (Rs + Rp) / 2;
        __declspec(align(16)) float refl_Intensity_1 = 1.0f - refl_Intensity;
 
        // RGBColor result = emissionColor + 
        //               *(t3)                   t3                   * refl_Intensity + t5                                          * refl_Intensity_1
        //     materialColor * (reflRay.traceForColor(surface, depth) * refl_Intensity + refracRay.traceForColor(surface, depth - 1) * (1 - refl_Intensity));

        __declspec(align(16)) RGBColor t3 = reflRay.traceForColor(surface, depth, diffuse_depth);
        // depth - 1 to favor refraction (refraction is always a two-step process)
        __declspec(align(16)) RGBColor t5 = refracRay.traceForColor(surface, depth - 1, diffuse_depth);
        t3 = t3 * refl_Intensity;
        t5 = t5 * refl_Intensity_1;
        t3 = t3 + t5;
        __declspec(align(16)) RGBColor t2 = materialColor * t3;
        __declspec(align(16)) RGBColor result = emissionColor + t2;

        return result.Trunc();
    }
    else
    {
        assert(0 && "Undefined reflType\n");
        return RGBColor(0.0f, 0.0f, 0.0f);
    }
};

// Return RGBColor based on one light source and shadow.
// Assumption: this ray points at a light source.
RGBColor Ray::traceForLight(const Surface& surface, const Surface *light) const
{
    // Find the light.
    float t;
    Surface *s = nullptr;

    if (surface.Hit(*this, RAY_T0, RAY_T1, &t, &s, nullptr) == false) {
        // Did not hit anything, but is not possible with this ray, the assumption is violated.
        std::cerr << "Error: Didn't hit anything as expected." << std::endl;
        return RGBColor(0.0f, 0.0f, 0.0f);
    }

    if (s == nullptr || s->GetMaterial() == nullptr) {
        // If reached here, somehow the material of this surface does not exist.
        // This should not happen.
        std::cerr << "Error: Didn't find light correctly as expect." << std::endl;
        return RGBColor(0.0f, 0.0f, 0.0f);
    }

    if (s != light) // something blocked
        return RGBColor();
    else
        return s->GetMaterial()->emissionColor;
}

inline float dot(const Vector3f& v1, const Vector3f& v2)
{
    __declspec(align(16)) float rDot = (v1.x * v2.x + v1.y * v2.y + v1.z * v2.z);
    return rDot;
}

inline Vector3f cross(const Vector3f& v1, const Vector3f& v2) {
    __declspec(align(16)) Vector3f prod;
    prod.x = v1.y * v2.z - v1.z * v2.y;
    prod.y = v1.z * v2.x - v1.x * v2.z;
    prod.z = v1.x * v2.y - v1.y * v2.x;
    return prod;
}

double get_wall_time() {
    LARGE_INTEGER time, freq;
    if (!QueryPerformanceFrequency(&freq)) {
        // Ignore error.
        return 0;
    }
    if (!QueryPerformanceCounter(&time)) {
        // Ignore error.
        return 0;
    }
    return (double)time.QuadPart / freq.QuadPart;
}

double get_cpu_time() {
    FILETIME a, b, c, d;
    if (GetProcessTimes(GetCurrentProcess(), &a, &b, &c, &d) != 0) {
        // Returns total user time.
        // Can be tweaked to include kernel times as well.
        return
            (double)(d.dwLowDateTime |
            ((unsigned long long)d.dwHighDateTime << 32)) * 0.0000001;
    }

    // Ignore error.
    return 0;
}

const int MESH_LINE_MAX = 255;   // max number of chars in one line in mesh file

int GetVertexIndexFromString(const std::string& str) {
	size_t index1 = string::npos;
	size_t index2 = string::npos;

	index1 = str.find('/');
	if (index1 != string::npos) {
		index2 = str.find('/', index1 + 1);
	}

	if (index1 == string::npos && index2 == string::npos) {
		return atoi(str.c_str()) - 1;
	}
	else {
		std::string str1 = str.substr(0, index1);
		return atoi(str1.c_str()) - 1;
	}
}

std::shared_ptr<Surface> LoadMesh(const char *file_name, float scale, const Vector3f& offset) {
	std::shared_ptr<Group> pMesh(new Group());

	FILE *fp;
	errno_t err = fopen_s(&fp, file_name, "r");
	char line[MESH_LINE_MAX];
	char delims[] = " ";
	char *type = NULL;
	char *param1 = NULL;
	char *param2 = NULL;
	char *param3 = NULL;
	char *next = NULL;

	float minX = std::numeric_limits<float>::infinity();
	float maxX = -std::numeric_limits<float>::infinity();
	float minY = std::numeric_limits<float>::infinity();
	float maxY = -std::numeric_limits<float>::infinity();
	float minZ = std::numeric_limits<float>::infinity();
	float maxZ = -std::numeric_limits<float>::infinity();

	if (err == 0) {
		std::vector<Point3f> positionList;

		while (fgets(line, MESH_LINE_MAX, fp) != NULL) {
			if (strlen(line) == 0 || line[0] == '#') {
				continue;
			}

			type = strtok_s(line, delims, &next);
			param1 = strtok_s(NULL, delims, &next);
			param2 = strtok_s(NULL, delims, &next);
			param3 = strtok_s(NULL, delims, &next);

			if (strcmp(type, "v") == 0) {          // A vertex
				float p1 = static_cast<float>(atof(param1)) * scale;
				float p2 = static_cast<float>(atof(param2)) * scale;
				float p3 = static_cast<float>(atof(param3)) * scale;

				p1 += offset.x;
				p2 += offset.y;
				p3 += offset.z;

				if (p1 < minX) minX = p1;
				if (p1 > maxX) maxX = p1;
				if (p2 < minY) minY = p2;
				if (p2 > maxY) maxY = p2;
				if (p3 < minZ) minZ = p3;
				if (p3 > maxZ) maxZ = p3;

				Point3f point(p1, p2, p3);
				positionList.push_back(point);
			}
			else if (strcmp(type, "f") == 0) {     // A face
				std::string face_str_1(param1);
				std::string face_str_2(param2);
				std::string face_str_3(param3);

				int vertexIndex1 = GetVertexIndexFromString(face_str_1);
				int vertexIndex2 = GetVertexIndexFromString(face_str_2);
				int vertexIndex3 = GetVertexIndexFromString(face_str_3);

				std::shared_ptr<Triangle> tri(new Triangle(positionList[vertexIndex1], positionList[vertexIndex2], positionList[vertexIndex3]));
				pMesh->AddObject(tri);
			}
			else {
				// Just ignore.
			}
		}

		// End of the file has been reached.
		fclose(fp);
	}
	else {
		perror("Error opening the mesh file");
		return nullptr;
	}

	Point3f center((minX + maxX) / 2.0f, (minY + maxY) / 2.0f, (minZ + maxZ) / 2.0f);
	float radius = (maxX - minX) > (maxY - minY) && (maxX - minX) > (maxZ - minZ) ? (maxX - minX) / 2.0f :
		(maxY - minY) > (maxZ - minZ) ? (maxY - minY) / 2.0f : (maxZ - minZ) / 2.0f;
	radius += 0.001f;

	pMesh->SetEnclosingSphere(center, radius);

	return pMesh;
}
