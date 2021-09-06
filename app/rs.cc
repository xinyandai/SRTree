#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "vecs.hh"
#include "stop_w.hh"
#include "record.hh"
#include "HnSRTreeFile.hh"


static void
storeSRTree(float *xb, const char *ft, int nb, int dimension, HnBool debug)
{
  HnSRTreeFile tree_file;
  HnPoint point;
  HnData data;
  RecordPoint record_point;
  RecordData record_data;
  FILE *fp;

  int dataSize = sizeof(RecordData);
  int blockSize = 1024 * dimension;

  int splitFactor = 40;
  int reinsertFactor = 30;

  tree_file = new_HnSRTreeFile(ft, dimension, dataSize, blockSize,
                               splitFactor, reinsertFactor);

  if(tree_file == HnSRTreeFile::null) {
    perror(ft);
    exit(1);
  }

  if(debug)
    tree_file.setDebug(HnTRUE);

  if(tree_file.getDimension() != dimension) {
    fprintf(stderr, "Mismatch in dimensions. "
                    "The dimension of the tree is %d.\n",
            tree_file.getDimension());
    exit(1);
  }

  ProgressBar bar(nb, "insert");
  const float* xi = xb;
  for (int i = 0; i < nb; ++i, ++bar, xi += dimension) {

    getRecord(xi, i, dimension, &record_point, &record_data);

    if(includesNANorINF(record_point))
      continue;

    point = new_HnPoint(dimension);
    for(i = 0; i < dimension; i++) {
      point.setCoord(record_point.coords[i], i);
    }

    data = new_HnData(&record_data, sizeof(record_data));

//    printf("Storing %s\n", (char *)point.toString());

    tree_file.store(point, data);

  }

  fclose(fp);
  tree_file.close();
}

int
main(int argc, char *argv[]) {

  if (argc != 3) {
    std::cout << "example: $./rs /path/to/vecs /path/to/tree\n";
    return 0;
  }

  HnBool debug = HnFALSE;

  char *fb = argv[1];
  char *ft = argv[2];

  int nb, dimension;
  float *xb;

  load_data(xb, dimension, nb, fb);

  storeSRTree(xb, ft, nb, dimension, debug);

  return 0;
}
