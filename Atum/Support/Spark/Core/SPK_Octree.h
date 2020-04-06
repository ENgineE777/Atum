//
// SPARK particle engine
//
// Copyright (C) 2008-2011 - Julien Fryer - julienfryer@gmail.com
// Copyright (C) 2017 - Frédéric Martin - fredakilla@gmail.com
//
// This software is provided 'as-is', without any express or implied
// warranty.  In no event will the authors be held liable for any damages
// arising from the use of this software.
//
// Permission is granted to anyone to use this software for any purpose,
// including commercial applications, and to alter it and redistribute it
// freely, subject to the following restrictions:
//
// 1. The origin of this software must not be misrepresented; you must not
//    claim that you wrote the original software. If you use this software
//    in a product, an acknowledgment in the product documentation would be
//    appreciated but is not required.
// 2. Altered source versions must be plainly marked as such, and must not be
//    misrepresented as being the original software.
// 3. This notice may not be removed or altered from any source distribution.
//

#ifndef H_SPK_OCTREE
#define H_SPK_OCTREE

#include <set>

namespace SPK
{
	/**
	* @brief A class representing an octree in a group space
	*
	* A octree is a data structure that partitions the space.<br>
	* When a octree is attached to a group, at each update, every individual particle is processed and put into cells.<br>
	* Particles become therefore aware of their neighbors.<br>
	* <br>
	* Octrees allows optimization of algorithms that run in O(n²) by reducing their complexity to O(nlog(n)).<br>
	* Typically algorithms where each particle is affected by every other particles in the group (particle vs particle collision, flocking, nbody simulations...).<br>
	* <br>
	* A Octree is automatically generated within a group if at least one of its modifiers needs it (by setting its NEEDS_OCTREE constant to true at init).<br>
	* If no more modifiers need an octree and an octree exists within the group, it is deleted.
	*/
	class SPK_PREFIX Octree
	{
	friend class Group;

	public :

        class Cell;

        // A fast and simple self reallocating array (faster than generic std::vectors)
		template<typename T>
		class Array
		{
		friend class Octree;
		friend class Cell;

		public :

			~Array<T>() { SPK_DELETE_ARRAY(values); }

			int size() const					{ return currentNb; }
			int capacity() const				{ return maxNb; }
			bool empty() const					{ return currentNb == 0; }
			T& operator[](int i)				{ return values[i]; }
			const T& operator[](int i) const	{ return values[i]; }

			void push(T t)
			{
				if (currentNb == maxNb)
				{
					// Reallocates
					maxNb <<= 1;
					T* tmp = SPK_NEW_ARRAY(T,maxNb);
					for (int i = 0; i < currentNb; ++i)
						tmp[i] = values[i];
					SPK_DELETE_ARRAY(values);
					values = tmp;
				}

				values[currentNb++] = t;
			}

			void clear() { currentNb = 0; }

		/*private*/public : // tmp workaround because I wasnt able to allows Cell to be friend of Array (compiler bug ?) but thats unsafe

			int currentNb;
			int maxNb;
			T* values;

			Array<T>(int maxNb = 1) :
				currentNb(0),
				maxNb(maxNb)
			{
				values = SPK_NEW_ARRAY(T,maxNb);
			}

			Array<T>(const Array<T>& t) :
				currentNb(t.currentNb),
				maxNb(t.maxNb)
			{
				values = SPK_NEW_ARRAY(T,maxNb);
				for (int i = 0; i < currentNb; ++i)
					values[i] = t.values[i];
			} // Not called

			// Not safe but optimized and hidden
			Array& operator=(const Array& t)
			{
				currentNb = t.currentNb;
				if (maxNb < currentNb) // Reallocates only if necessary
				{
					maxNb = t.maxNb;
					SPK_DELETE_ARRAY(values);
					values = SPK_NEW_ARRAY(T,maxNb);
				}
				for (int i = 0; i < currentNb; ++i)
					values[i] = t.values[i];
				return *this;
			}
		};


		class Cell
		{
		friend class Octree;
		friend class Array<Cell>;

		public :

			int level;
			int offsetX;
			int offsetY;
			int offsetZ;
			int children[8];
			bool hasChildren;
			Array<int> particles;

		private :

			void init(int level, int offsetX, int offsetY, int offsetZ)
			{
				this->level = level;
				this->offsetX = offsetX;
				this->offsetY = offsetY;
				this->offsetZ = offsetZ;
				hasChildren = false;
				particles.clear();
			}

			Cell(int level = 0, int offsetX = 0, int offsetY = 0, int offsetZ = 0) :
				level(level),
				offsetX(offsetX),
				offsetY(offsetY),
				offsetZ(offsetZ),
				hasChildren(false),
				particles(MAX_PARTICLES_NB_PER_CELL)
			{}

			// Not safe but optimized and hidden
			Cell(const Cell& cell) :
				level(cell.level),
				offsetX(cell.offsetX),
				offsetY(cell.offsetY),
				offsetZ(cell.offsetZ),
				hasChildren(false),
				particles(MAX_PARTICLES_NB_PER_CELL)
			{} // Not called

			// Not safe but optimized and hidden
			Cell& operator=(const Cell& cell)
			{
				level = cell.level;
				offsetX = cell.offsetX;
				offsetY = cell.offsetY;
				offsetZ = cell.offsetZ;
				hasChildren = cell.hasChildren;
				std::memcpy(children,cell.children,8 * sizeof(int));
				particles = cell.particles;
				return *this;
			}
		};

		/**
		* @brief Gets the array of active cells
		* Active cells are cells of the octree that contains at least one particle
		* @return active cells of the octree
		*/
		const Array<int>& getActiveCells() const							{ return activeCells; }

		/**
		* @brief Gets the neighboring cells of a given particle
		* The neighboring cells of a particle are the cells in which lie the particle.<br>
		* Note the index of the particle passed also lies within each of its neighboring cells.<br>
		* Note also that particles are guaranteed to be ordered by index within a given cell.
		* @param particleIndex : the index of the particle
		* @return the neighboring cells of a given particle
		*/
		const Array<int>& getNeighborCells(int particleIndex) const	{ return particleCells[particleIndex]; }

		/**
		* @brief Gets a given cell by index
		* @param index : the index of the cell
		* @return the cell at the given index
		*/
		const Cell& getCell(int index) const								{ return cells[index]; }

		/**
		* @brief Gets the minimum position of the axis aligned bounding box of the octree
		* @return the minimum position of the AABB
		*/
		const Vector3D& getAABBMin() const { return AABBMin; }

		/**
		* @brief Gets the maximum position of the axis aligned bounding box of the octree
		* @return the maximum position of the AABB
		*/
		const Vector3D& getAABBMax() const { return AABBMax; }

		// TODO Allows to tweak the octree param

	private :

		struct Triplet
		{
			int value[3];

			void set(const Vector3D& v)
			{
				value[0] = static_cast<int>(v.x);
				value[1] = static_cast<int>(v.y);
				value[2] = static_cast<int>(v.z);
			}
		};

		static const int MAX_LEVEL_INDEX;
		static const int MAX_PARTICLES_NB_PER_CELL;
		static const float OPTIMAL_CELL_SIZE_FACTOR;
		static const float MIN_CELL_SIZE;

		Group& group;

		Array<Cell> cells; // Pool of cells
		int nbCells;

		Array<int> activeCells; // Index of the active cells

		Array<int>* particleCells; // Cells to which particles belongs
		int nbParticles;

		Triplet* minPos;
		Triplet* maxPos;

		Vector3D AABBMin;
		Vector3D AABBMax;

		// Octree life time is managed by Group
		Octree(const Ref<Group>& group);
		~Octree();

		Octree(const Octree& octree); // never used
		Octree& operator=(const Octree& octree); // never used

		void update();  // Used by Group only

		int initNextCell(int level, int offsetX, int offsetY, int offsetZ);
		void addToCell(int cellIndex, int particleIndex, int maxLevel);
		void addToChildrenCells(int parentIndex, int particleIndex, int maxLevel);
	};
}

#endif
