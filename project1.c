//gcc project1.c -o project1.out -I/usr/include/libxml2 -lxml2

#include "stdio.h"
#include "libxml/parser.h"
#include "libxml/tree.h"
#include "split.h"
#include "libxml/xmlschemastypes.h"

#define KRED  "\x1B[31m"  //RED
#define KBLU  "\x1B[34m"  //BLUE
#define KNRM  "\x1B[0m"   //BLACK

void xsdValidation(char *xsdName, char *xmlName){
  const char *xmlLint = "xmllint --noout --schema ";

  char *terminalCommand = malloc(strlen(xmlLint) + strlen(xsdName) + strlen(xmlName) + 1);
  if(terminalCommand != NULL){
    strcpy(terminalCommand, xmlLint); /* use strcpy() here to initialize the result buffer */
    strcat(terminalCommand, xsdName);
    strcat(terminalCommand, " ");
    strcat(terminalCommand, xmlName);
    system(terminalCommand);
  }
}

static void xmlWalk(xmlNode *a_node){
  xmlNode *currentNode = NULL;     //node
  xmlAttr *NodeAttribute = NULL;   //attribute as an object
  xmlChar *attribute;              //char or char array
  for (currentNode = a_node; currentNode; currentNode = currentNode->next) {
    if (currentNode->type == XML_ELEMENT_NODE) {
      printf("Node Name : %-15s", currentNode->name);
      if(currentNode->properties != NULL){
        printf("\n %20s Attributes\n","");
        for (NodeAttribute = currentNode->properties; NodeAttribute; NodeAttribute = NodeAttribute->next) {
          printf("%s", NodeAttribute->name);
          attribute =  xmlNodeGetContent((xmlNode*)NodeAttribute);
          printf(" = %s\n", attribute);
        }
      }
      printf("Value = %s\n", currentNode->children->content);
    }
    xmlWalk(currentNode->children);
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
    xmlWalk(root_element);
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
}

void menu(const char *commands[]){
  char *xmlName;
  char *xsdName;
  char **parsedInput;
  int i = 0;

  for (i = 0; i < sizeof(commands); i++) {
    if(strcmp(commands[1],"chartgen") == 0){
      if(strcmp(commands[i],"-i") == 0){
        parsing(parsedInput,(char *)commands[i+1],".");
        if(strcmp(parsedInput[1],"xml")==0){
          xmlName = malloc(strlen((char *)commands[(i+1)]) + 5);
          strcpy(xmlName, strcat((char *)commands[(i+1)],".xml"));
          parseXML(xmlName);
        }
      }

      if(strcmp(commands[i],"-o") == 0){
        parsing(parsedInput,(char *)commands[i+1],".");
        if(strcmp(parsedInput[1],"svg")==0){
          printf("%s\n", "NOT READY");
        }
      }

      if(strcmp(commands[i],"-v") == 0){
        parsing(parsedInput,(char *)commands[i+1],".");
        if(strcmp(parsedInput[1],"xsd")==0){
          xsdName = malloc(strlen((char *)commands[(i+1)]) + 5);
          strcpy(xsdName, strcat((char *)commands[(i+1)],".xsd"));
          xsdValidation(xsdName,xmlName);
          free(xsdName);
          free(xmlName);
        }
      }

      if(strcmp(commands[i],"-t")==0 && (strcmp(commands[i+1],"line")==0 || strcmp(commands[i+1],"pie")==0  || strcmp(commands[i+1],"bar")==0)){
        printf("%s\n", "NOT READY");

      }

      if(strcmp(commands[i],"-h")==0){
        help();
        return;
      }
    }
    else{
      help();
      return;
    }
  }
}

int main(int argc, char const *argv[]) {
  menu(argv);
  return 0;
}
