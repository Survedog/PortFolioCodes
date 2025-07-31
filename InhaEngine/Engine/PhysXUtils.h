#pragma once

#include "PxConfig.h"
#include "foundation/PxVec3.h"
#include "foundation/PxQuat.h"
#include "foundation/PxSimpleTypes.h"

inline PxVec3 Vec3ToPxVec3(const Vec3& v) { return PxVec3(v.x, v.y, v.z); }
inline Vec3 PxVec3ToVec3(const PxVec3& pv) { return Vec3(pv.x, pv.y, pv.z); }
inline PxQuat QuatToPxQuat(const Quaternion& q) { return PxQuat(q.x, q.y, q.z, q.w); }
inline Quaternion PxQuatToQuat(const PxQuat& pq) { return Quaternion(pq.x, pq.y, pq.z, pq.w); }

inline void SwizzleVectorXZY(PxVec3& vec) { swap(vec.y, vec.z); }
inline PxVec3 GetSwizzledVectorXZY(const PxVec3& vec) { PxVec3 ret = vec; SwizzleVectorXZY(ret);  return ret; }

inline void SwizzleVectorYXZ(PxVec3& vec) { swap(vec.x, vec.y); }
inline PxVec3 GetSwizzledVectorYXZ(const PxVec3& vec) { PxVec3 ret = vec; SwizzleVectorYXZ(ret);  return ret; }

inline void SwizzleVectorZYX(PxVec3& vec) { swap(vec.x, vec.z); }
inline PxVec3 GetSwizzledVectorZYX(const PxVec3& vec) { PxVec3 ret = vec; SwizzleVectorZYX(ret);  return ret; }

inline void SwizzleVectorYZX(PxVec3& vec) { swap(vec.x, vec.z); swap(vec.x, vec.y); }
inline PxVec3 GetSwizzledVectorYZX(const PxVec3& vec) { PxVec3 ret = vec; SwizzleVectorYZX(ret);  return ret; }

inline void SwizzleVectorZXY(PxVec3& vec) { swap(vec.x, vec.z); swap(vec.y, vec.z); }
inline PxVec3 GetSwizzledVectorZXY(const PxVec3& vec) { PxVec3 ret = vec; SwizzleVectorZXY(ret);  return ret; }

inline void ConvertGameCoordsToPxCoords(PxVec3& vec) { SwizzleVectorYZX(vec); }
inline void ConvertGameCoordsToPxCoords(Vec3& vec) { SwizzleVectorYZX(vec); }
inline PxVec3 GetPxCoordsFromGameCoords(const PxVec3& vec) { return GetSwizzledVectorYZX(vec); }
inline PxVec3 GetPxCoordsFromGameCoords(const Vec3& vec) { PxVec3 ret = Vec3ToPxVec3(vec); ConvertGameCoordsToPxCoords(ret); return ret; }

inline void ConvertPxCoordsToGameCoords(PxVec3& vec) { SwizzleVectorZXY(vec); }
inline void ConvertPxCoordsToGameCoords(Vec3& vec) { SwizzleVectorZXY(vec); }
inline Vec3 GetGameCoordsFromPXCoords(const Vec3& vec) { return GetSwizzledVectorZXY(vec); }
inline Vec3 GetGameCoordsFromPXCoords(const PxVec3& vec) { Vec3 ret = PxVec3ToVec3(vec); ConvertPxCoordsToGameCoords(ret); return ret; }
