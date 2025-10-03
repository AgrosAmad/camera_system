#include <XMath.cuh>
#include <Grid1D.h>
#include <Grid2D.h>
#include <Grid3D.h>

namespace XM
{
    void SetGridSize(std::vector<dim3> &gridBlock, const int3 &dim)
    {
		// Prepares data
		dim3 blockSize = gridBlock.back();
		dim3 gridSize;

		// Computes thread size per dimension
		gridSize.x = DivUp(dim.x, blockSize.x);
		gridSize.y = DivUp(dim.y, blockSize.y);
		gridSize.z = DivUp(dim.z, blockSize.z);

		gridBlock = { gridSize, blockSize };
    }

	void SetGridSize(std::vector<dim3>& gridBlock, const int2 &dim)
	{
		// Prepares data
		dim3 blockSize = gridBlock.back();
		dim3 gridSize;

		// Computes thread size per dimension
		gridSize.x = DivUp(dim.x, blockSize.x);
		gridSize.y = DivUp(dim.y, blockSize.y);
		gridSize.z = 1;

		gridBlock = { gridSize, blockSize };
	}

    void SetGridSize(int2 &gridBlock, const int &dim)
    {
		gridBlock.x = DivUp(dim, gridBlock.y);
    }

    int DivUp(const int &a, const int &b)
    {
		return (a % b != 0) ? (a / b + 1) : (a / b);
    }

    int SelectCudaDevice()
    {
		int deviceCount;
		cudaGetDeviceCount(&deviceCount);
		int device = deviceCount - 1;
		checkCudaErrors(cudaSetDevice(device));
		return device;
    }
    
	__host__ __device__ Domain1D::Domain1D(X3::geom::Grid1D &grid)
    {
		// Main data from grid
		Xa = grid.BoundingBox().LimitsX[0];
		Xb = grid.BoundingBox().LimitsX[1];
		Dim = grid.Nx();

		// Computes Ds
		Dx = (Xb - Xa) / (float)Dim;

		// Computes inverse squared ds
		DxInvSqr = 1.f / (Dx * Dx);
    }

	__host__ __device__ Domain2D::Domain2D(X3::geom::Grid3D& grid, const bool& useXZ)
	{
		// Main data from grid
		if (useXZ)
		{
			Dim = make_int2(grid.Nx(), grid.Nz());
			A.x = grid.BoundingBox().LimitsX[0];
			A.y = grid.BoundingBox().LimitsZ[0];
			B.x = grid.BoundingBox().LimitsX[1];
			B.y = grid.BoundingBox().LimitsZ[1];
		}
		else
		{
			Dim = make_int2(grid.Nx(), grid.Ny());
			A.x = grid.BoundingBox().LimitsX[0];
			A.y = grid.BoundingBox().LimitsY[0];
			B.x = grid.BoundingBox().LimitsX[1];
			B.y = grid.BoundingBox().LimitsY[1];
		}

		// Computes Ds
		Ds = (B - A) / make_float2(Dim);

		// Computes inverse squared ds
		DsInvSqr = 1.f / (Ds * Ds);

		// Total domain size
		Size = Dim.x * Dim.y;
	}

    __host__ __device__ Domain2D::Domain2D(X3::geom::Grid2D &grid)
    {
		// Main data from grid
		Dim = make_int2(grid.Nx(), grid.Ny());
		A.x = grid.BoundingBox().LimitsX[0];
		A.y = grid.BoundingBox().LimitsY[0];
		B.x = grid.BoundingBox().LimitsX[1];
		B.y = grid.BoundingBox().LimitsY[1];

		// Computes Ds
		Ds = (B - A) / make_float2(Dim);

		// Computes inverse squared ds
		DsInvSqr = 1.f / (Ds * Ds);

		// Total domain size
		Size = Dim.x * Dim.y;
    }

	__host__ __device__ Domain3D::Domain3D(X3::geom::Grid3D& grid)
	{
		// Main data from grid
		Dim = make_int3(grid.Nx(), grid.Ny(), grid.Nz());
		A.x = grid.BoundingBox().LimitsX[0];
		A.y = grid.BoundingBox().LimitsY[0];
		A.z = grid.BoundingBox().LimitsZ[0];
		B.x = grid.BoundingBox().LimitsX[1];
		B.y = grid.BoundingBox().LimitsY[1];
		B.z = grid.BoundingBox().LimitsZ[1];

		// Computes Ds
		Ds = (B - A) / make_float3(Dim);

		// Computes inverse squared ds
		DsInvSqr = 1.f / (Ds * Ds);

		// Total domain size
		Size = Dim.x * Dim.y * Dim.z;
	}

} // namespace XMath