// Material properties.
#ifndef _MATERIAL_H
#define _MATERIAL_H

#include "SimpleImage.h"

enum class Type : char {
    LIGHT,      // light
    DIFF,        // diffuse
    SPEC,        // specular
    REFR,        // refraction
};

class Material {

public:
    __declspec(align(16)) RGBColor materialColor;
    __declspec(align(16)) RGBColor emissionColor;
    __declspec(align(16)) float diffAmount;
    __declspec(align(16)) float refrIndex;
	__declspec(align(16)) float extRefrIndex;
	Type reflType;

    Material() {
        reflType = Type::DIFF;
        diffAmount = 1.0f;
        refrIndex = 1.5f;
		extRefrIndex = 1.0f;
	}

    Material(const RGBColor& _mColor) {
        materialColor = _mColor;
        reflType = Type::DIFF;
        diffAmount = 1.0f;
        refrIndex = 1.5f;
		extRefrIndex = 1.0f;
	}

    Material(const RGBColor& _mColor, const RGBColor& _eColor) {
        materialColor = _mColor;
        emissionColor = _eColor;
        reflType = Type::DIFF;
        diffAmount = 1.0f;
        refrIndex = 1.5f;
		extRefrIndex = 1.0f;
	}

    void SetEmissionColor(const RGBColor& _eColor) {
        emissionColor = _eColor;
    }

    void SetReflectionType(Type t) {
        reflType = t;
    }

    void SetDiffAmount(float diff) {
        diffAmount = diff;
    }

    void SetRefrIndex(float _refr, float _ext) {
        refrIndex = _refr;
		extRefrIndex = _ext;
	}

    bool fIsReflective() const {
        return materialColor.r != 0 || materialColor.g != 0 || materialColor.b != 0;
    }

    void* operator new(size_t i)
    {
        return _mm_malloc(i, 16);
    }

    void operator delete(void* p)
    {
        _mm_free(p);
    }

};

#endif
