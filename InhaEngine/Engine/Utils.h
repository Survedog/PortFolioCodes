#pragma once

#ifndef UTILS_H
#define UTILS_H

#define ENGINE (Engine::GetInstance())

// Error handling
class Exception : std::exception
{
public:
	Exception(const std::wstring& functionName, const std::wstring& fileName, int lineNumber, const std::wstring& userMessage = L"None")
		: _functionName(functionName), _fileName(fileName), _userMessage(userMessage), _lineNumber(lineNumber)
	{
	}

	virtual char const* what() const override;
	virtual std::wstring what_utf8() const;

	std::wstring _functionName;
	std::wstring _fileName;
	std::wstring _userMessage;
	int _lineNumber;
};

class IOException : public Exception
{
public:
	IOException(const std::wstring& functionName, const std::wstring& fileName, int lineNumber)
		: Exception(functionName, fileName, lineNumber)
	{
	}

	IOException(const std::wstring& functionName, const std::wstring& fileName, int lineNumber, const std::wstring& userMessage)
		: Exception(functionName, fileName, lineNumber, userMessage)
	{
	}
};

class PhysXException : public Exception
{
public:
	PhysXException(const std::wstring& functionName, const std::wstring& fileName, int lineNumber)
		: Exception(functionName, fileName, lineNumber)
	{
	}

	PhysXException(const std::wstring& functionName, const std::wstring& fileName, int lineNumber, const std::wstring& userMessage)
		: Exception(functionName, fileName, lineNumber, userMessage)
	{
	}
};

class DXException : public Exception
{
public:
	DXException(HRESULT hr, const std::wstring& functionName, const std::wstring& fileName, int lineNumber)
		: Exception(functionName, fileName, lineNumber), _errorCode(hr)
	{
	}

	DXException(HRESULT hr, const std::wstring& functionName, const std::wstring& fileName, int lineNumber, const std::wstring& userMessage)
		: Exception(functionName, fileName, lineNumber, userMessage), _errorCode(hr)
	{
	}

	virtual std::wstring what_utf8() const override;

	HRESULT _errorCode;
};

#define THROW_EXCEPTION(msg)												\
{																			\
	std::wstring wFunc = AnsiToWString(__FUNCTION__);						\
	std::wstring wFile = AnsiToWString(__FILE__);							\
	throw Exception(wFunc, wFile, __LINE__, (msg));							\
}

#define THROW_IO_EXCEPTION(msg)												\
{																			\
	std::wstring wFunc = AnsiToWString(__FUNCTION__);						\
	std::wstring wFile = AnsiToWString(__FILE__);							\
	throw Exception(wFunc, wFile, __LINE__, (msg));							\
}

// TODO: Use PhysX Error Callback
#define THROW_PHYSX															\
{																			\
    std::wstring wFunc = AnsiToWString(__FUNCTION__);						\
	std::wstring wFile = AnsiToWString(__FILE__);							\
    throw PhysXException(wFunc, wFile, __LINE__);							\
}

#define THROW_MSG_PHYSX(msg)												\
{																			\
    std::wstring wFunc = AnsiToWString(__FUNCTION__);						\
	std::wstring wFile = AnsiToWString(__FILE__);							\
    throw PhysXException(wFunc, wFile, __LINE__, (msg));					\
}

#define THROW_DX_IF_FAILED(x)												\
{																			\
    HRESULT hr__ = (x);														\
    std::wstring wFunc = AnsiToWString(__FUNCTION__);						\
	std::wstring wFile = AnsiToWString(__FILE__);							\
    if(FAILED(hr__)) { throw DXException(hr__, wFunc, wFile, __LINE__); }	\
}

#define THROW_MSG_DX_IF_FAILED(x, msg)												\
{																					\
    HRESULT hr__ = (x);																\
    std::wstring wFunc = AnsiToWString(__FUNCTION__);								\
	std::wstring wFile = AnsiToWString(__FILE__);									\
    if(FAILED(hr__)) { throw DXException(hr__, wFunc, wFile, __LINE__, (msg)); }	\
}

// MISC
#define IMPL_SINGLETON_NO_CONSTRUCTOR(type)			\
public:												\
	static type* GetInstance()						\
	{												\
		static type instance;						\
		return &instance;							\
	}												\
private:											\
	type(const type&) = delete;						\
	type(const type&&) = delete;					\
	type& operator=(const type&) = delete;			\
	type& operator=(type&&) = delete;

#define IMPL_SINGLETON(type)			\
	IMPL_SINGLETON_NO_CONSTRUCTOR(type)	\
	type() = default;					

inline std::wstring AnsiToWString(const std::string& str)
{
	return std::wstring(str.begin(), str.end());
}

inline std::string WStringToAnsi(const std::wstring& wstr)
{
	return std::string(wstr.begin(), wstr.end());
}

inline const DirectX::SimpleMath::Vector3 operator*(const DirectX::SimpleMath::Vector3& v, const DirectX::SimpleMath::Matrix& m)
{
	const XMVECTOR vector = XMLoadFloat3(&v);
	const XMMATRIX matrix = XMLoadFloat4x4(&m);
	const XMVECTOR ret = XMVector3Transform(vector, m);

	XMFLOAT3 r;
	XMStoreFloat3(&r, ret);
	return r;
}

inline XMVECTOR XM_CALLCONV GetSwizzledVectorXZY(FXMVECTOR vec) { return XMVectorSwizzle<XM_SWIZZLE_X, XM_SWIZZLE_Z, XM_SWIZZLE_Y, XM_SWIZZLE_W>(vec); }
inline void XM_CALLCONV SwizzleVectorXZY(XMFLOAT3& vec) { swap(vec.y, vec.z); }

inline XMVECTOR XM_CALLCONV GetSwizzledVectorYXZ(FXMVECTOR vec) { return XMVectorSwizzle<XM_SWIZZLE_Y, XM_SWIZZLE_X, XM_SWIZZLE_Z, XM_SWIZZLE_W>(vec); }
inline void XM_CALLCONV SwizzleVectorYXZ(XMFLOAT3& vec) { swap(vec.x, vec.y); }

inline XMVECTOR XM_CALLCONV GetSwizzledVectorZYX(FXMVECTOR vec) { return XMVectorSwizzle<XM_SWIZZLE_Z, XM_SWIZZLE_Y, XM_SWIZZLE_X, XM_SWIZZLE_W>(vec); }
inline void XM_CALLCONV SwizzleVectorZYX(XMFLOAT3& vec) { swap(vec.x, vec.z); }

inline XMVECTOR XM_CALLCONV GetSwizzledVectorYZX(FXMVECTOR vec) { return XMVectorSwizzle<XM_SWIZZLE_Y, XM_SWIZZLE_Z, XM_SWIZZLE_X, XM_SWIZZLE_W>(vec); }
inline void XM_CALLCONV SwizzleVectorYZX(XMFLOAT3& vec) { swap(vec.x, vec.z); swap(vec.x, vec.y); }

inline XMVECTOR XM_CALLCONV GetSwizzledVectorZXY(FXMVECTOR vec) { return XMVectorSwizzle<XM_SWIZZLE_Z, XM_SWIZZLE_X, XM_SWIZZLE_Y, XM_SWIZZLE_W>(vec); }
inline void XM_CALLCONV SwizzleVectorZXY(XMFLOAT3& vec) { swap(vec.x, vec.z); swap(vec.y, vec.z); }

inline XMVECTOR XM_CALLCONV GetDxCoordsFromGameCoords(FXMVECTOR vec) { return GetSwizzledVectorYZX(vec); }
inline void XM_CALLCONV ConvertGameCoordsToDxCoords(XMFLOAT3& vec) { SwizzleVectorYZX(vec); }

inline XMVECTOR XM_CALLCONV GetGameCoordsFromDXCoords(FXMVECTOR vec) { return GetSwizzledVectorZXY(vec); }
inline void XM_CALLCONV ConvertDXCoordsToGameCoords(XMFLOAT3& vec) { SwizzleVectorZXY(vec); }

inline DirectX::SimpleMath::Vector3 RadianVectorToDegree(const DirectX::SimpleMath::Vector3& rv)
{
	return DirectX::SimpleMath::Vector3(rv.x, rv.y, rv.z) * 180.f / XM_PI;
}

inline DirectX::SimpleMath::Vector3 DegreeVectorToRadian(const DirectX::SimpleMath::Vector3& ev)
{
	return DirectX::SimpleMath::Vector3(ev.x, ev.y, ev.z) * XM_PI / 180.f;
}

inline bool IsNearlyZero(const DirectX::SimpleMath::Vector3& v, const float threshold = 0.001f)
{
	return DirectX::XMVectorGetX(DirectX::XMVector3LengthSq(XMLoadFloat3(&v))) <= (threshold * threshold);
}

inline int GetVectorSum(const DirectX::XMINT3& v) { return v.x + v.y + v.z; }
inline int GetVectorSum(const DirectX::XMINT4& v) { return v.x + v.y + v.z + v.w; }
inline unsigned int GetVectorSum(const DirectX::XMUINT3& v) { return v.x + v.y + v.z; }
inline unsigned int GetVectorSum(const DirectX::XMUINT4& v) { return v.x + v.y + v.z + v.w; }
inline float GetVectorSum(const DirectX::XMFLOAT3& v) { return v.x + v.y + v.z; }
inline float GetVectorSum(const DirectX::XMFLOAT4& v) { return v.x + v.y + v.z + v.w; }

inline void SetVectorElementByIdx(DirectX::XMINT3& outV, int value, unsigned int elementIdx)
{
	if (elementIdx == 0) outV.x = value;
	else if (elementIdx == 1) outV.y = value;
	else if (elementIdx == 2) outV.z = value;
	else assert(false && "Element index must be under 3.");
}

inline void SetVectorElementByIdx(DirectX::XMINT4& outV, int value, unsigned int elementIdx)
{
	if (elementIdx == 0) outV.x = value;
	else if (elementIdx == 1) outV.y = value;
	else if (elementIdx == 2) outV.z = value;
	else if (elementIdx == 3) outV.w = value;
	else assert(false && "Element index must be under 4.");
}

inline void SetVectorElementByIdx(DirectX::XMUINT3& outV, unsigned int value, unsigned int elementIdx)
{
	if (elementIdx == 0) outV.x = value;
	else if (elementIdx == 1) outV.y = value;
	else if (elementIdx == 2) outV.z = value;
	else assert(false && "Element index must be under 3.");
}

inline void SetVectorElementByIdx(DirectX::XMUINT4& outV, unsigned int value, unsigned int elementIdx)
{
	if (elementIdx == 0) outV.x = value;
	else if (elementIdx == 1) outV.y = value;
	else if (elementIdx == 2) outV.z = value;
	else if (elementIdx == 3) outV.w = value;
	else assert(false && "Element index must be under 4.");
}

inline void SetVectorElementByIdx(DirectX::SimpleMath::Vector3& outV, float value, unsigned int elementIdx)
{
	if (elementIdx == 0) outV.x = value;
	else if (elementIdx == 1) outV.y = value;
	else if (elementIdx == 2) outV.z = value;
	else assert(false && "Element index must be under 3.");
}

inline void SetVectorElementByIdx(DirectX::SimpleMath::Vector4& outV, float value, unsigned int elementIdx)
{
	if (elementIdx == 0) outV.x = value;
	else if (elementIdx == 1) outV.y = value;
	else if (elementIdx == 2) outV.z = value;
	else if (elementIdx == 3) outV.w = value;
	else assert(false && "Element index must be under 4.");
}

inline DirectX::SimpleMath::Vector3 GetNormalizedVector(const DirectX::SimpleMath::Vector3& v)
{
	XMVECTOR nv = DirectX::XMVector3Normalize(XMLoadFloat3(&v));
	return nv;
}

inline XMMATRIX XM_CALLCONV InverseTranspose(FXMMATRIX matrix)
{
	XMMATRIX m = matrix;
	m.r[3] = XMVectorSet(0.f, 0.f, 0.f, 1.f);

	XMVECTOR det = ::XMMatrixDeterminant(m);
	return XMMatrixTranspose(XMMatrixInverse(&det, m));
}

// TODO: Check if it's actually getting inlined.
template<typename T>
inline const T* GetConstPtr(T* ptr) { return static_cast<const T*>(ptr); }

template<typename T>
inline shared_ptr<const T> GetConstPtr(shared_ptr<T> ptr) { return const_pointer_cast<const T>(ptr); }

#define CONST_THIS GetConstPtr(this)

#endif //UTILS_H
