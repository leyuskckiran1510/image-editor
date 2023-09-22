
#include "image-editor.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>


/*
  [Wiki ] https://en.wikipedia.org/wiki/JPEG

*/
void binary(unsigned short int x,char *final){
  int p=0;
  do {
      final[p++]=x&1?'1':'0';
  }while (x>>=1);
  final[p]='\0';
}

void jpeg_display(JPEG j){
  // char final[10];
  // binary(1,final);
  // printf("%s \n",final);
  printf("%s %u \n","soi",j.soi>>8);
  printf("%s %u \n","marker",j.marker>>8);
  printf("%s %d \n","length",j.length>>8);
  printf("%s %s \n","identifier",j.identifier);
  printf("%s %d \n","version",j.version);
  printf("%s %d \n","density_unit",j.density_unit);
  printf("%s %d \n","xdensity",j.xdensity>>8);
  printf("%s %d \n","ydensity",j.ydensity>>8);
  printf("%s %d \n","xthumbnail",j.xthumbnail);
  printf("%s %d \n","ythumbnail",j.ythumbnail);
}

void jpeg_cast(FILE *fp,JPEG *j){
  int d=0;
  d =fread_unlocked(&j->soi,1,2,fp);
  d =fread_unlocked(&j->marker,1,2,fp);
  d =fread_unlocked(&j->length,1,2,fp);
  d =fread_unlocked(&j->identifier,5,1,fp);
  d =fread_unlocked(&j->version,1,2,fp);
  d =fread_unlocked(&j->density_unit,1,1,fp);
  d =fread_unlocked(&j->xdensity,2,1,fp);
  d =fread_unlocked(&j->ydensity,2,1,fp);
  d =fread_unlocked(&j->xthumbnail,1,1,fp);
  d =fread_unlocked(&j->ythumbnail,1,1,fp);
  if(d){
    return;
  }
}


void load_image(const char *file_name) {
  FILE *f = fopen(file_name, "rb");
  if (f == NULL) {
    log("Failed To Open the Image File %s", file_name);
    exit(1);
  }
  JPEG *temp=malloc(sizeof(JPEG));
  jpeg_cast(f,temp);
  jpeg_display(*temp);
  free(temp);
}

int main(void) {
  load_image("./src/test.jpg");
  return 0;
}