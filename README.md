## SR-Tree

* based on HnSRTree: the SR-Tree library, ersion 1.3.1 12/02/97 katayama@rd.nacsis.ac.jp

* How to compile:

```shell
mkdir build -p && cd build
cmake .. && make -j 
```

    
* How to use:

    Sorry, no documentation is available. Please, refer to the
    following descriptions and the sample programs contained in
    the `app' directory.

    (1) header files

        (C  ) HnSRTree.h

        (C++) HnSRTreeFile.hh

        This library has two language interfaces:
        the C language interface and the C++ language interface.
        The header file `HnSRTree.h' is provided for the C users
        and `HnSRTreeFile.hh' is for the C++ users.

    (2) libraries

        (C and C++) libHnSRTree.a

        Although this library has the C language interface, the
        body of this library is written in C++. Therefore, a C++
        linker is required to link your object files with this
        library even if your object files are compiled only with
        the C compiler.

    (3) creating an SR-tree file
	
        (C  ) HnSRTree *HnSRTreeCreate(const char *path,
                                int dimension,
                                int dataSize,
                                int blockSize,
                                int splitFactor,
                                int reinsertFactor
                                );

            (C++) HnSRTreeFile new_HnSRTreeFile(const char *path,
                                                int dimension,
                                                int dataSize,
                                                int blockSize,
                                                int splitFactor,
                                                int reinsertFactor
                                                );

        These functions create an SR-tree file and return the
        pointer to a data structure associated with that file.

            path           : the name of an SR-tree file
                dimension      : the dimensionality of the search space
        dataSize       : the size of an attribute associated
                 with each leaf entry
        blockSize      : the size of a block (or bucket)
                 (typically 8192)
        splitFactor    : the minimum utilization factor of a
                 block (specified in percent)
                 (typically 40)
        reinsertFactor : the percentage of entries to be reinserted
                 on a forced reinsertion
                 (typically 30)

        The function `new_HnSRTreeFile()' of the C++ interface is
        not equivalent to the expression `new HnSRTreeFile()' 
        which invokes the `new' operator. `new_HnSRTreeFile()' is
        a special function to instantiate an object of the class
        `HnSRTreeFile'. This library takes the Java semantics for
            object instantiation. The variables are allocated only for
            referencing objects and objects are instantiated only by
            special constructors whose name starts with the string
            `new_'. The instantiated objects are automatically
            reclaimed on losing references to themselves.

    (4) opening an SR-tree for reading or updating

        (C  ) HnSRTree *HnSRTreeUpdate(const char *path);
          HnSRTree *HnSRTreeOpen(const char *path);      

        (C++) HnSRTreeFile new_HnSRTreeFile(const char *path,
                        const char *mode);

        These functions open an SR-tree file for reading or updating.

        path : the name of an SR-tree file
        mode : the access mode for opening an SR-tree file

        In the C language interface, the function `HnSRTreeUpdate()'
        opens an SR-tree file for updating. On the other hand, the
        function `HnSRTreeOpen()' opens an SR-tree file for reading.

        Upon successful completion, both return the pointer to a
        data structure associated with that file. Otherwise,
        a null pointer is returned and errno is set to indicate
        the error.

        In the C++ language interface, the variable `mode'
        specifies the access mode for opening an SR-tree file.
        Mode "r" is for reading and mode "rw" is for updating. 

        Upon successful completion, the function returns an object
        of the class `HnSRTreeFile' which is associated with that
        opened file. Otherwise, the constant `HnSRTreeFile::null'
        is returned and errno is set to indicate the error.

    (5) closing an SR-tree file

        (C  ) void HnSRTreeClose(HnSRTree *file);

        (C++) void file.close(void);

        These functions close an SR-tree file.

        file : the pointer to a data structure associated with
               an SR-tree file

    (6) storing a point into an SR-tree file.

        (C  ) void HnSRTreeStore(HnSRTree *file,
                     const double coords[],
                     const void *ptr,
                     int size
                     );

        (C++) void file.store(const HnPoint &point,
                  const HnData &data
                  );

        These functions store a point into an SR-tree file. The
        current implementation imposes no restriction for
        duplication. This reduces the overhead on insertion.
        The same point-attribute pairs can be duplicated.
        Duplication does not harm the functionality of this
        library.

        file   : the pointer to a data structure associated with
                 an SR-tree file
        coords : the coordinates of a point to be stored
        ptr    : the pointer to an attribute to be stored
             with a point
        size   : the size of an attribute

        point  : a point being stored
        data   : an attribute being stored

        In the C++ language interface, the class `HnPoint' and
        `HnData' are used. An object of the class `HnPoint' can
        be created in the following way:

        HnPoint point;

        point = new_HnPoint(dimension);
        for (i=0; i<dimension; i++)
            point.setCoord(coords[i], i);

        where it is supposed that the dimensionality of the object
        is given by the variable `dimension' and its coordinates
        are given by the array `coords[]'.

        An object of the class `HnData' can be created in the
        following way:

        HnData data;

              data = new_HnData(ptr, size);

        where it is supposed that the content of the object is
        given by a byte string whose location is `ptr' and whose
        size is `size'.

    (7) removing a point from an SR-tree file.

        (C  ) void HnSRTreeRemove(HnSRTree *file,
                      const double coords[],
                      const void *ptr,
                      int size
                      );

        (C++) void file.remove(const HnPoint &point, const HnData &data);

        These functions remove a point from an SR-tree file. Only
        a point having the same coordinates and the same attribute
        is removed. Points with different attributes are intact.

        file   : the pointer to a data structure associated with
             an SR-tree file
        coords : the coordinates of a point to be removed
        ptr    : the pointer to an attribute to be removed
             with a point
        size   : the size of an attribute to be removed

        point  : a point being removed
        data   : an attribute being removed

    (8) running a nearest neighbor search

        (C  ) void HnSRTreeGetNeighbors(HnSRTree *file,
                        const double coords[],
                        int numNeighbors,
                        HnSRTreeRecord **records_return,
                        int *numRecords_return
                        );

        (C++) void file.getNeighbors(const HnPoint &point,
                     int numNeighbors,
                     HnPointArray *points_return,
                     HnDataArray *values_return
                     );

        These functions run a nearest neighbor search, i.e., they
        search the given number of points that are the nearest to
        a given point.

        file              : the pointer to a data structure
                    associated with an SR-tree file
        coords            : the coordinates of a target point
            numNeighbors      : the number of points to be found
        records_return    : records of nearest points
        numRecords_return : the number of nearest points

        point             : the target point
        points_return     : nearest points
        values_return     : attributes of nearest points

            The `numNeighbors' is not necessarily the maximum number
        of points to be obtained. If the farthest point of a
        result set has multiple points at the same rank, they are
        also included in the result set.

        In the C language interface, the type `HnSRTreeRecord' is
            used. It is defined in the header file `HnSRTree.h' as
        follows:

        typedef struct {
                double *coords;
                void *data;
                int size;
        } HnSRTreeRecord;

        where `coords' is an array of coordinates of a point,
        `data' is the pointer to its attribute, and `size' is the
        size of the attribute. Records being returned by the
        function `HnSRTreeGetNeighbors()' are allocated by that
        function and released on its next invocation.

        For example, the coordinates and the attributes of nearest
        points can be obtained as follows:

        HnSRTree *file;
        double coords[DIMENSION];
        int numNeighbors;
        HnSRTreeRecord *records;
        int numRecords;
        int i, j;

        ...

        (open an SR-tree and set `coords' and `numNeighbors')

        ...

        HnSRTreeGetNeighbors(file, coords, numNeighbors,
                     &records, &numRecords);

        for (i=0; i<numRecords; i++) {
            double coords[DIMENSION];
            void *data;
            int size;

            for (j=0; j<DIMENSION; j++) {
                coords[j] = records[i].coords[j];
                ....
            }
            ....
            data = records[i].data;	
            size = records[i].size;
            ....
        }

        In the C++ language interface, the class `HnPointArray'
        and `HnDataArray' are used. The former is the array of
        instances of the class `HnPoint' and the latter is the
        array of instances of the class `HnData'.

        For example, the coordinates and the attributes of nearest
        points can be obtained in the following way:

        HnSRTreeFile file;
        HnPoint target;
        int numNeighbors;
        HnPointArray points;
        HnDataArray values;
        int i, j;

        ...

        (open an SR-tree and set `target' and `numNeighbors')

        ...

        file.getNeighbors(target, numNeighbors, &points, &values);

        for (i=0; i<points.length(); i++) {
            double coords[DIMENSION];
                   char *data;
            int size;

            for (j=0; j<DIMENSION; j++) {
                coords[j] = points[i].getCoord(j);
                ....
            }
            ....
            data = values[i].chars();
            size = values[i].length();
            ....
        }

    (9) running a range search

        (C  ) void HnSRTreeGetFirst(HnSRTree *file,
                    const HnSRTreeRange ranges[],
                        HnSRTreeRecord **record_return
                    );
              void HnSRTreeGetNext(HnSRTree *file,
                            HnSRTreeRecord **record_return
                       );

        (C++) void file.getFirst(HnPoint *point_return,
                                 HnData *value_return
                         );
          void file.getFirst(const HnRect &region,
                                     HnPoint *point_return,
                     HnData *value_return
                     );
          void file.getNext(HnPoint *point_return,
                        HnData *value_return
                        );

        These functions run a range search, i.e., they search
        points within a given region.

        file          : the pointer to a data structure associated
                                with an SR-tree file
        ranges        : the array of a range of the target region
        record_return : a record of an obtained point

        region        : the target region
        point_return  : an obtained point
        value_return  : an attribute of an obtained point

        In the C language interface, the type `HnSRTreeRange' is
            used. It is defined in the header file `HnSRTree.h' as
        follows:

        typedef struct {
                double min;
                double max;
        } HnSRTreeRange;

        where `min' is the lowest bound and `max' is the highest
        bound of a range. A record being returned by the function
        `HnSRTreeGetFirst()' and `HnSRTreeGetNext()' is allocated
        by these functions and released on their next invocation. 

        For example, the coordinates and the attributes of points
        within a region can be obtained as follows:

        HnSRTree *file;
        HnSRTreeRange ranges[DIMENSION];
        HnSRTreeRecord *record;
        int i;

        ...

        (open an SR-tree and set `ranges')

        ...

        HnSRTreeGetFirst(file, ranges, &record);

        while (record != NULL) {
            double coords[DIMENSION];
            void *data;
            int size;

            for (i=0; i<DIMENSION; i++) {
                coords[i] = record->coords[i];
                ....
            }
            ....
            data = record->data;	
            size = record->size;
            ....

            HnSRTreeGetNext(file, &record);
        }

        If the NULL is given to the second argument of
        `HnSRTreeGetFirst()', every point in a tree is obtained.

        In the C++ language interface, the class `HnRect' is
        used. An object of the class `HnRect' can be created in
        the following way:

        HnRect rect;

        rect = new_HnRect(dimension);
        for (i=0; i<dimension; i++)
            rect.setRange(min[i], HnRange::INCLUSIVE,
                      max[i], HnRange::INCLUSIVE,
                      i);

        where it is supposed that the dimensionality of the object
        is given by the variable `dimension' and its range of each
        dimension is given by the array `min[]' and `max[]'.

        For example, the coordinates and the attributes of points
        within a region can be obtained in the following way:

        HnSRTreeFile file;
        HnRect region;
        HnPoint point;
        HnData value;
        int i;

        ...

        (open an SR-tree and set `region')

        ...

        file.getFirst(region, &point, &value);

        while (point != HnPoint::null) {
            double coords[DIMENSION];
            char *data;
            int size;

            for (i=0; i<DIMENSION; i++) {
                coords[i] = point.getCoord(i);
                ....
            }
            ....
            data = value.chars();
            size = value.length();
            ....

            file.getNext(&point, &value);
        }

        If the function `getFirst(&point, &values)' is used
        instead of the function `getFirst(rect, &point, &value)',
        every point in a tree is obtained.


History:

    10/15/96
	Version 1.0 is released.

    11/22/96
	The range query functions, HnSRTreeGetFirst() and
	HnSRTreeGetNext(), is added to the C language interface.

    11/22/96
	Version 1.1 is released.

    12/09/96
	The access mode is permitted and the diagnosis is returned on
	opening files.

    12/09/96
	Version 1.2 is released.

    06/02/97
	The GNU Library General Public License is applied.

    06/02/97
	Version 1.3 is released.

    12/02/97
	The file `README' is slightly modified.
	Version 1.3.1 is released.
