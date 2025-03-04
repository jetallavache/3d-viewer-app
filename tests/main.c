#include "../kernel/utils.h"
#include "../kernel/dataset.h"
#include "../kernel/transform.h"

#include <stdio.h>

int main() {
  char *name  = "test-sample.obj";  
  FILE *f = NULL;  

  WireframeModel *model = malloc(1 * sizeof(WireframeModel));      
  int status = kernel_utils_open_file(&f, name);  
  if (!status) status = kernel_dataset_file_to_object(f, model);     
  
  kernel_utils_close_file(&f);
  kernel_dataset_free(&model);
  
  return 0;
}