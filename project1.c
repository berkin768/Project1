//gcc project1.c -o project1.out -I/usr/include/libxml2 -lxml2

#include "stdio.h"
#include "string.h"
#include "stdlib.h"
#include "libxml/parser.h"
#include "libxml/tree.h"
#include "split.h"

#define KRED  "\x1B[31m"  //RED
#define KBLU  "\x1B[34m"  //BLUE
#define KNRM  "\x1B[0m"   //BLACK

static void print_element_names(xmlNode *a_node){
  xmlNode *cur_node = NULL;
  xmlNode *cur_attr = NULL;
  for (cur_node = a_node; cur_node; cur_node = cur_node->next) {
    if (cur_node->type == XML_ELEMENT_NODE) {
      // printf("Node Type: Element, name: %s \n Its children's type is: %d \n Its children's content is: %s \n", cur_node->name, cur_node->children->type, cur_node->children->content);

      printf("Node Name : %-20s", cur_node->name);
      if(cur_node->properties != NULL){
        // for (cur_attr = cur_node->properties; cur_attr; cur_attr = cur_attr->next) {
        //   printf("  -> with attribute : %s\n", cur_attr->name);
        // }
      }
      printf("Content %s\n", cur_node->children->content);
    }
    print_element_names(cur_node->children);
  }
}

void parseXML(char *xmlName){
  xmlDoc *doc = NULL;
  xmlNode *root_element = NULL;
  const char *Filename = xmlName;

  doc = xmlReadFile(Filename, NULL, 0);

  if(doc == NULL){
    printf("Error: couldn't parse file%s\n", Filename);
  }
  else{
    root_element = xmlDocGetRootElement(doc);
    print_element_names(root_element);
    xmlFreeDoc(doc);
  }
  xmlCleanupParser();
}

void help(){
  printf("%s%s%s\n",KBLU,"INPUTS ARE :  ", KNRM);
  printf("%s\n","1- 'chartgen -i <input filename>'");
  printf("%s\n","2- 'chartgen -o <output filename>'");
  printf("%s\n","3- 'chartgen -v <xsd validation file>'");
  printf("%s\n","4- 'chartgen -t <type>'");
  printf("%s\n","5- 'clear'");
}

void menu(){
  char command[100];
  char *parsedInput[50];
  printf("%s%s%s",KRED,"COMMAND > ", KNRM);
  scanf("%49[^\n]%*c", command);
  parsing(parsedInput, command, " ");

  if(strcmp(parsedInput[0],"chartgen") == 0){ //command must start with chartgen
    if(strcmp(parsedInput[1],"-h") == 0){ //if our second letter is -h
      help();  //goes to help function
    }
    if(strcmp(parsedInput[1],"-i")==0){
      parseXML(parsedInput[2]);
    }
    if(strcmp(parsedInput[1],"-o")==0){
      printf("%s\n", "NOT READY");
    }
    if(strcmp(parsedInput[1],"-v")==0){
      printf("%s\n", "NOT READY");
    }
    if(strcmp(parsedInput[1],"-t")==0){
      printf("%s\n", "NOT READY");
    }
  }
  else if(strcmp(parsedInput[0],"clear")==0){   //if our command is just clear
    system("clear");  //clears
  }
}

int main(int argc, char const *argv[]) {
  /* code */
  int exit = 0;

  printf("%s%s%s\n",KBLU,"Type 'HELP' for commands", KNRM);
  while(exit == 0){
    menu();
  }

  return 0;
}
