#pragma once

// CUDA
#include <curand_kernel.h>
#include <cuda_runtime.h>
#include <helper_math.h>
#include <helper_cuda.h>
#include <curand.h>

// Thrust
#include <thrust/device_malloc.h>
#include <thrust/device_vector.h>
#include <thrust/host_vector.h>
#include <thrust/extrema.h>

// GL
#include <glm/gtc/matrix_transform.hpp>
#include <glm/glm.hpp>
#include <glad/glad.h>
#include <GLFW/glfw3.h>

// STL
#include <type_traits>
#include <cstdint>
#include <iostream>
#include <fstream>
#include <vector>

// Forward declaration
namespace X3
{
	namespace geom
	{
		class Grid1D;
		class Grid2D;
		class Grid3D;
	}
}

// Out of class overloads

// Constructors
// glm::vec3 to float3
__host__ __device__ inline float3 make_float3(const glm::vec3& vec)
{
	return ::make_float3(vec.x, vec.y, vec.z);
}

// float3 to glm::vec3
__host__ __device__ inline glm::vec3 make_vec3(const float3& vec)
{
	return glm::vec3(vec.x, vec.y, vec.z);
}

// glm::vec3 to float3
__host__ __device__ inline float2 make_float2(const glm::vec2& vec)
{
	return ::make_float2(vec.x, vec.y);
}

// float3 to glm::vec3
__host__ __device__ inline glm::vec2 make_vec2(const float2& vec)
{
	return glm::vec2(vec.x, vec.y);
}

// dim3 constructor
__host__ __device__ inline dim3 make_dim3(const int& dim)
{
	return dim3(dim, dim, dim);
}

// Operators
__host__ __device__ inline bool operator!=(const int2& a, const int2& b) 
{
	return (a.x != b.x) || (a.y != b.y);
}

__host__ __device__ inline float2 operator*(const int2& a, const float2& b)
{
	return make_float2(static_cast<float>(a.x) * b.x, static_cast<float>(a.y) * b.y);
}

__host__ __device__ inline float2 operator+(const int2& a, const float2& b)
{
	return make_float2(static_cast<float>(a.x) + b.x, static_cast<float>(a.y) + b.y);
}

__host__ __device__ inline float2 operator*(const float2& a, const int2& b)
{
    return b * a;
}

__host__ __device__ inline float3 operator*(const int3& a, const float3& b)
{
	return make_float3(static_cast<float>(a.x) * b.x, static_cast<float>(a.y) * b.y, static_cast<float>(a.z) * b.z);
}

__host__ __device__ inline float3 operator*(const float3& a, const int3& b)
{
	return b * a;
}

__host__ __device__ inline int2 floor(const float2& value)
{
	return make_int2(floor(value.x), floor(value.y));
}

__host__ __device__ inline int3 floor(const float3& value)
{
	return make_int3(floor(value.x), floor(value.y), floor(value.z));
}

namespace XM
{

	// Constants
	constexpr float PI = 3.14159265358979323846f;
	constexpr float EPSILON = 1e-6f;

	// Useful structs

	// Axis aligned bounding box
	struct AABB
	{
		float LimitsX[2], LimitsY[2], LimitsZ[2];
	
		// Default constructor
		__host__ __device__ AABB() {}
	
		// Constructor to initialize the limits
		__host__ __device__ AABB(float minX, float maxX, float minY, float maxY, float minZ, float maxZ)
		{
			LimitsX[0] = minX; LimitsX[1] = maxX;
			LimitsY[0] = minY; LimitsY[1] = maxY;
			LimitsZ[0] = minZ; LimitsZ[1] = maxZ;
		}
	
		// Constructor to initialize a cube with a single min and max
		__host__ __device__ AABB(float min, float max)
		{
			LimitsX[0] = min; LimitsX[1] = max;
			LimitsY[0] = min; LimitsY[1] = max;
			LimitsZ[0] = min; LimitsZ[1] = max;
		}
	
		// Constructor to initialize a cube with a single min/max
		__host__ __device__ AABB(float minMax)
		{
			LimitsX[0] = -minMax; LimitsX[1] = minMax;
			LimitsY[0] = -minMax; LimitsY[1] = minMax;
			LimitsZ[0] = -minMax; LimitsZ[1] = minMax;
		}
	
		// Overloaded + operator for adding two AABBs
		__host__ __device__ AABB operator+(const AABB& other) const
		{
			return AABB(LimitsX[0] + other.LimitsX[0], LimitsX[1] + other.LimitsX[1],
						LimitsY[0] + other.LimitsY[0], LimitsY[1] + other.LimitsY[1],
						LimitsZ[0] + other.LimitsZ[0], LimitsZ[1] + other.LimitsZ[1]);
		}
	};
	

	// Simulation domains
	struct Domain1D
	{
		float DxInvSqr;
		float Xa, Xb;
		float Dx;
		int Dim;

		// Default constructor
		__host__ __device__ Domain1D() {}

		// Constructor for 3D domain
		__host__ __device__ Domain1D(const float& xa, const float& xb, const int& nx) : Xa(xa), Xb(xb), Dim(nx)
		{
			// Computes Ds
			Dx = (Xb - Xa) / (float)Dim;

			// Computes inverse squared ds
			DxInvSqr = 1.f / (Dx * Dx);
		}

		// Constructor from grids
		__host__ __device__ Domain1D(X3::geom::Grid1D &grid);
	};

	struct Domain2D
	{
		float2 DsInvSqr;
		float2 A, B;
		float2 Ds;
		int Size;
		int2 Dim;

		// Default constructor
		__host__ __device__ Domain2D() {}

		// Constructor for 3D domain
		__host__ __device__ Domain2D(const float& xa, const float& xb, const float& ya, const float& yb, const int& nx)
		{
			// Sets limits
			Dim = make_int2(nx, nx);
			A = make_float2(xa, ya);
			B = make_float2(xb, yb);

			// Computes Ds
			Ds = (B - A) / make_float2(Dim);

			// Computes inverse squared ds
			DsInvSqr = 1.f / (Ds * Ds);

			// Total domain size
			Size = Dim.x * Dim.y;
		}

		__host__ __device__ Domain2D operator*(float factor) const
		{
			Domain2D result;
			result.A = A;
			result.B = B;

			// Scale up the resolution by multiplying dimensions
			result.Dim = make_int2(
				static_cast<int>(Dim.x * factor),
				static_cast<int>(Dim.y * factor)
			);

			// Recompute voxel size
			float2 extent = B - A;
			result.Ds = extent / make_float2(result.Dim);
			result.DsInvSqr = 1.f / (result.Ds * result.Ds);
			result.Size = result.Dim.x * result.Dim.y;

			return result;
		}

		// Constructor from grids
		__host__ __device__ Domain2D(X3::geom::Grid3D& grid, const bool& useXZ = true);
		__host__ __device__ Domain2D(X3::geom::Grid2D &grid);
	};

	struct Domain3D
	{
		float3 DsInvSqr;
		float3 A, B;
		float3 Ds;
		int Size;
		int3 Dim;

		// Default constructor
		__host__ __device__ Domain3D() {}

		// Constructor for 3D domain
		__host__ __device__ Domain3D(const float& xa, const float& xb, const float& ya, const float& yb, const float& za, const float& zb, const int& nx)
		{
			// Sets limits
			Dim = make_int3(nx, nx, nx);
			A = make_float3(xa, ya, za);
			B = make_float3(xb, yb, zb);

			// Computes Ds
			Ds = (B - A) / make_float3(Dim);

			// Computes inverse squared ds
			DsInvSqr = 1.f / (Ds * Ds);

			// Total domain size
			Size = Dim.x * Dim.y * Dim.z;
		}

		// Constructor from grids
		__host__ __device__ Domain3D(X3::geom::Grid3D& grid);

		__host__ __device__ Domain3D operator*(float factor) const
		{
			Domain3D result;
			result.A = A;
			result.B = B;

			// Scale up the resolution by multiplying dimensions
			result.Dim = make_int3(
				static_cast<int>(Dim.x * factor),
				static_cast<int>(Dim.y * factor),
				static_cast<int>(Dim.z * factor)
			);

			// Recompute voxel size
			float3 extent = B - A;
			result.Ds = extent / make_float3(result.Dim);
			result.DsInvSqr = 1.f / (result.Ds * result.Ds);
			result.Size = result.Dim.x * result.Dim.y * result.Dim.z;

			return result;
		}

	};


	// Useful functions

	// Misc

	// Domain clamping
	__host__ __device__ inline float Clamp1D(const float& value, const float& min, const float& max)
	{
		return fmaxf(min, fminf(value, max));
	}

	__host__ __device__ inline int Clamp1D(const int& value, const int& minVal, const int& maxVal)
	{
		return max(minVal, min(value, maxVal));
	}

	__host__ __device__ inline float2 Clamp2D(const float2& value, const float2& min, const float2& max)
	{
		return make_float2(fmaxf(min.x, fminf(value.x, max.x)), fmaxf(min.y, fminf(value.y, max.y)));
	}

	__host__ __device__ inline int2 Clamp2D(const int2& value, const int2& minVal, const int2& maxVal)
	{
		return make_int2(max(minVal.x, min(value.x, maxVal.x)), max(minVal.y, min(value.y, maxVal.y)));
	}

	__host__ __device__ inline float3 Clamp3D(const float3& value, const float3& min, const float3& max)
	{
		return ::make_float3(fmaxf(min.x, fminf(value.x, max.x)), fmaxf(min.y, fminf(value.y, max.y)), fmaxf(min.z, fminf(value.z, max.z)));
	}

	__host__ __device__ inline int3 Clamp3D(const int3& value, const int3& minValue, const int3& maxValue)
	{
		return ::make_int3(max(minValue.x, min(value.x, maxValue.x)), max(minValue.y, min(value.y, maxValue.y)), max(minValue.z, min(value.z, maxValue.z)));
	}

	// Interpolation
	template <typename T, typename U>
	__host__ __device__ T LinearInterpolate(const T& v0, const T& v1, const U& alpha) 
	{
		return v0 * (U(1) - alpha) + v1 * alpha;
	}

	template <typename T, typename U>
	__host__ __device__ T CubicInterpolate(const T& q_im1, const T& q_i, const T& q_ip1, const T& q_ip2, const U& s) 
	{
		// Precompute coefficients
		U s2 = s * s;
		U s3 = s2 * s;

		// Compute weights
		U w_im1 = (-1.0 / 3.0) * s + 0.5 * s2 - (1.0 / 6.0) * s3;
		U w_i = 1.0 - s2 + 0.5 * (s3 - s);
		U w_ip1 = s + 0.5 * (s2 - s3);
		U w_ip2 = (1.0 / 6.0) * (s3 - s);

		// Return the weighted sum
		return w_im1 * q_im1 + w_i * q_i + w_ip1 * q_ip1 + w_ip2 * q_ip2;
	}

	template <typename T, typename U>
	__host__ __device__ T BilinearInterpolate(
		const T& v00, const T& v01,
		const T& v10, const T& v11,
		const U& alpha)
	{
		T bottom = LinearInterpolate(v00, v10, alpha.x);
		T top = LinearInterpolate(v01, v11, alpha.x);
		return LinearInterpolate(bottom, top, alpha.y);
	}

	template <typename T, typename U>
	__host__ __device__ T BicubicInterpolate(
		const T& q_im1_jm1, const T& q_i_jm1, const T& q_ip1_jm1, const T& q_ip2_jm1,
		const T& q_im1_j,   const T& q_i_j,   const T& q_ip1_j,   const T& q_ip2_j,
		const T& q_im1_jp1, const T& q_i_jp1, const T& q_ip1_jp1, const T& q_ip2_jp1,
		const T& q_im1_jp2, const T& q_i_jp2, const T& q_ip1_jp2, const T& q_ip2_jp2,
		const U& s)
	{
		// Interpolate along i (x) for each j-row
		T row_jm1 = CubicInterpolate(q_im1_jm1, q_i_jm1, q_ip1_jm1, q_ip2_jm1, s.x);
		T row_j   = CubicInterpolate(q_im1_j,   q_i_j,   q_ip1_j,   q_ip2_j,   s.x);
		T row_jp1 = CubicInterpolate(q_im1_jp1, q_i_jp1, q_ip1_jp1, q_ip2_jp1, s.x);
		T row_jp2 = CubicInterpolate(q_im1_jp2, q_i_jp2, q_ip1_jp2, q_ip2_jp2, s.x);
	
		// Interpolate along j (y)
		return CubicInterpolate(row_jm1, row_j, row_jp1, row_jp2, s.y);
	}

	template <typename T, typename U>
	__host__ __device__ T TrilinearInterpolate(
		const T& v000, const T& v001,
		const T& v010, const T& v011,
		const T& v100, const T& v101,
		const T& v110, const T& v111,
		const U& alpha)
	{
		// Interpolate along x at z = 0
		T c00 = LinearInterpolate(v000, v100, alpha.x);
		T c01 = LinearInterpolate(v001, v101, alpha.x);
		T c10 = LinearInterpolate(v010, v110, alpha.x);
		T c11 = LinearInterpolate(v011, v111, alpha.x);

		// Interpolate along y at z = 0 and z = 1
		T c0 = LinearInterpolate(c00, c10, alpha.y);
		T c1 = LinearInterpolate(c01, c11, alpha.y);

		// Final interpolation along z
		return LinearInterpolate(c0, c1, alpha.z);
	}

	

	// Cuda dimensions
	void SetGridSize(std::vector<dim3>& gridBlock, const int3& dim); // computes block size for cuda 3D
	void SetGridSize(std::vector<dim3>& gridBlock, const int2& dim); // computes block size for cuda 2D
	void SetGridSize(int2& gridBlock, const int& dim); // computes block size for cuda 1D
	int DivUp(const int& a, const int& b);  // computes block size for cuda
	int SelectCudaDevice(); // selects the best cuda device

	// Indexing
	__host__ __device__ inline float3 IdxToPos(const int3& idx, const Domain3D& domain)
	{
		return idx * domain.Ds;
	}

	__host__ __device__ inline int Index1D(const int3& idx, const int& sizeX, const int& sizeY)
	{
		return idx.x + idx.y * sizeX + idx.z * sizeX * sizeY;
	}

	__host__ __device__ inline int Index1D(const int3& idx, const int3& gridDim)
	{
		return idx.x + idx.y * gridDim.x + idx.z * gridDim.x * gridDim.y;
	}

	__host__ __device__ inline int3 Index3D(const int& idx, const int& sizeX, const int& sizeY)
	{
		int3 result;
		result.z = idx / (sizeX * sizeY);
		result.y = (idx / sizeX) % sizeY;
		result.x = idx % sizeX;
		return result;
	}

	__host__ __device__ inline int3 Index3D(const int& idx, const int3& gridDim)
	{
		int3 result;
		result.z = idx / (gridDim.x * gridDim.y);
		result.y = (idx / gridDim.x) % gridDim.y;
		result.x = idx % gridDim.x;
		return result;
	}

	__host__ __device__ inline int2 Index2D(const int& idx, const int2& gridDim)
	{
		int2 result;
		result.y = idx / gridDim.x;       // Row index (y-coordinate)
		result.x = idx % gridDim.x;       // Column index (x-coordinate)
		return result;
	}

	__host__ __device__ inline int Index1D(const int2& idx, const int2& gridDim)
	{
		return idx.y * gridDim.x + idx.x;  // 1D index = row * width + column
	}

	// Exporting / Importing

	template <typename T>
	void PrintArray(T* array, int xdim, int ydim) {
		for (int i = xdim - 1; i >= 0; --i) { // last row to first
			for (int j = 0; j < ydim; ++j) {
				if constexpr (std::is_same_v<T, int>) {
					printf("%d ", array[i * ydim + j]);
				}
				else if constexpr (std::is_same_v<T, double>) {
					printf("%.3f ", array[i * ydim + j]);
				}
				else 
				{
					printf("%.3f ", array[i * ydim + j]);
				}
			}
			printf("\n");
		}
	}

	template <typename T>
	void PrintVector(const std::vector<T>& vector, int xdim, int ydim)
	{
		PrintArray(vector.data(), xdim, ydim);
	}

	template <typename T>
	void PrintArray(const T* array, size_t size)
	{
		for (int i = 0; i < size; ++i) {
			if constexpr (std::is_same_v<T, int>) {
				printf("%d ", array[i]);
			}
			else if constexpr (std::is_same_v<T, double>) {
				printf("%.6f ", array[i]);
			}
			else if constexpr (std::is_same_v<T, float>) {
				printf("%f ", array[i]);
			}
			else {
				// Fallback for unsupported types
				printf("<?> ");
			}
		}

		printf("\n");
	}

	template <typename T>
	void PrintVector(const std::vector<T>& vector)
	{
		PrintArray(vector.data(), vector.size());
	}

	template <typename T>
	void ArrayToBin(const std::string& filename, const T* data, int3 n)
	{
		std::ofstream out(filename, std::ios::binary);
		if (!out) throw std::runtime_error("Failed to open file for writing: " + filename);

		// Save dimensions (nx, ny, nz)
		out.write(reinterpret_cast<const char*>(&n), sizeof(int3));

		// Save array data
		size_t totalSize = static_cast<size_t>(n.x) * n.y * n.z;
		out.write(reinterpret_cast<const char*>(data), totalSize * sizeof(T));
	}

	template <typename T>
	void SaveArray(const T* array, size_t size, const std::string& filename) 
	{
		std::ofstream file(filename + ".txt");
		if (!file) {
			std::cerr << "Error: Could not open file " << filename << " for writing.\n";
			return;
		}

		for (size_t i = 0; i < size; ++i) {
			file << array[i] << "\n";
		}

		file.close();
	}

	template <typename T>
	void SaveVector(const std::vector<T>& vector, const std::string& filename)
	{
		SaveArray(vector.data(), vector.size(), filename);
	}

	template <typename T>
	void SaveArray2D(
		const T* array, size_t size,
		const std::string& filename,
		const int2& dim,            // dim = (dimx, dimy)
		const float2& origin,
		const float2& ds)
	{
		std::ofstream file(filename + ".txt");
		if (!file) {
			std::cerr << "Error: Could not open file " << filename << " for writing.\n";
			return;
		}

		// Write metadata header
		file << "# dim_x dim_y\n"
			<< dim.x << " " << dim.y << "\n";

		file << "# origin_x origin_y\n"
			<< origin.x << " " << origin.y << "\n";

		file << "# dx dy\n"
			<< ds.x << " " << ds.y << "\n";

		// Write flattened data
		for (size_t i = 0; i < size; ++i) {
			file << array[i] << "\n";
		}

		file.close();
	}

	template <typename T>
	void SaveVector2D(
		const std::vector<T>& vector,
		const std::string& filename,
		const int2& dim,
		const float2& origin,
		const float2& ds)
	{
		SaveArray2D(vector.data(), vector.size(), filename, dim, origin, ds);
	}

	template <typename T>
	void SaveArray3D(
		const T* array, size_t size,
		const std::string& filename,
		const int3& dim,           // (dimx, dimy, dimz)
		const float3& origin,
		const float3& ds)
	{
		std::ofstream file(filename + ".txt");
		if (!file) {
			std::cerr << "Error: Could not open file " << filename << " for writing.\n";
			return;
		}

		// Write metadata header
		file << "# dim_x dim_y dim_z\n"
			<< dim.x << " " << dim.y << " " << dim.z << "\n";

		file << "# origin_x origin_y origin_z\n"
			<< origin.x << " " << origin.y << " " << origin.z << "\n";

		file << "# dx dy dz\n"
			<< ds.x << " " << ds.y << " " << ds.z << "\n";

		// Write flattened data
		for (size_t i = 0; i < size; ++i) {
			file << array[i] << "\n";
		}

		file.close();
	}

	template <typename T>
	void SaveVector3D(
		const std::vector<T>& vector,
		const std::string& filename,
		const int3& dim,
		const float3& origin,
		const float3& ds)
	{
		SaveArray3D(vector.data(), vector.size(), filename, dim, origin, ds);
	}
}