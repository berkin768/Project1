//gcc project1.c -o project1.out -I/usr/include/libxml2 -lxml2
//./project1.out chartgen -v validation.xsd -i chartgendata.xml -o abc.svg -t pie

#include "stdio.h"
#include "libxml/xmlschemastypes.h"
#include "libxml/parser.h"
#include "libxml/tree.h"
#include "split.h"

#define KBLU  "\x1B[34m"  //BLUE
#define KNRM  "\x1B[0m"   //BLACK
#define TRUE 1
#define FALSE 0

char *chartTitle;

struct canvas{
  char *length;
  char *width;
  char backcolor[6];
};

struct Yaxis{
  char *name;
};

struct Xaxis{
  char *name;
  char forecolor[6];
};

struct Xset{
  char *xData[12];  //not dynamic
};

struct Yset{
  char *unit;
  char *name;
  char showValue[3];
  char fillColor[6];
  char *yData[12];  //not dynamic, but it might be problem for us
};

struct canvas svg_canvas;
struct Yaxis svg_yaxis;
struct Xaxis svg_xaxis;
struct Xset svg_xset;
struct Yset svg_ysets[6];  //not dynamic

int xmlFlag = FALSE;
int xsdFlag = FALSE;
int SVGEntered = FALSE;
int circleSVG = FALSE;
int barSVG = FALSE;
int lineSVG = FALSE;

int ySetCounter = 0;
int yDataCounter = 0;
int xDataCounter = 0;

void createCircleSVG(char *svgName){
  xmlDocPtr svg = NULL;
  xmlNodePtr body = NULL;
  xmlNodePtr charttitle = NULL, canvas = NULL, yaxis = NULL, xaxis = NULL, xset = NULL, yset = NULL;
  xmlNodePtr charttitleChild = NULL, canvasChild = NULL, yaxisChild = NULL, xaxisChild = NULL, xsetChild = NULL, ysetChild = NULL;

  svg = xmlNewDoc(BAD_CAST "1.0");

  body = xmlNewNode(NULL, BAD_CAST"body");
  xmlDocSetRootElement(svg, body);

  char *height = malloc(10);
  sprintf(height,"%d",(atoi(svg_canvas.length) * (ySetCounter+1)));

  canvas = xmlNewChild(body, NULL, BAD_CAST"svg", NULL);
  xmlNewProp(canvas,BAD_CAST "xmlns","http://www.w3.org/2000/svg");
  xmlNewProp(canvas, BAD_CAST "height", height);
  xmlNewProp(canvas, BAD_CAST "width", svg_canvas.width);
  xmlNewProp(canvas, BAD_CAST "fill", svg_canvas.backcolor);

  free(height);

  int i = 0;
  char *cy = malloc(5);

  for (i = 0; i <= ySetCounter; i++) {   //how many circle?
    sprintf(cy, "%d", (60+i*150));  //dynamic y coordinate
    canvasChild=xmlNewChild(canvas, NULL, BAD_CAST "circle", NULL);

    xmlNewProp(canvasChild, BAD_CAST "cx", BAD_CAST "60");
    xmlNewProp(canvasChild, BAD_CAST "cy", cy);
    xmlNewProp(canvasChild, BAD_CAST "r", BAD_CAST "50");
    xmlNewProp(canvasChild, BAD_CAST "stroke", BAD_CAST "black");
    xmlNewProp(canvasChild, BAD_CAST "stroke-width", BAD_CAST "2");
    xmlNewProp(canvasChild, BAD_CAST "fill", BAD_CAST "blue");
  }
  free(cy);

  charttitle = xmlNewChild(canvas, NULL, BAD_CAST "text", chartTitle);
  xmlNewProp(charttitle, BAD_CAST "x", BAD_CAST "10");
  xmlNewProp(charttitle, BAD_CAST "y", BAD_CAST "130");

  htmlSaveFileEnc(svgName, svg, "UTF-­8", 1);

  xmlFreeDoc(svg);

}

void xsdValidation(char *xsdName, char *xmlName){
  char xmlLint[30];
  strcpy(xmlLint,"xmllint --noout --schema ");

  char *terminalCommand = malloc(strlen(xmlLint) + strlen(xsdName) + strlen(xmlName) + 2);
  if(terminalCommand != NULL){
    strcpy(terminalCommand, xmlLint); /* use strcpy() here to initialize the result buffer */
    strcat(terminalCommand, xsdName);
    strcat(terminalCommand, " ");
    strcat(terminalCommand, xmlName);
    system(terminalCommand);
    free(terminalCommand);
  }
}

static void xmlWalk(xmlNode *a_node){
  xmlNode *currentNode = NULL;     //node
  xmlAttr *NodeAttribute = NULL;   //attribute as an object
  xmlChar *attribute;              //char or char array
  for (currentNode = a_node; currentNode; currentNode = currentNode->next) {
    if (currentNode->type == XML_ELEMENT_NODE) {

      if(strcmp(currentNode->name,"charttitle")==0){
        chartTitle = malloc(strlen(currentNode->children->content)+1);
        strcpy(chartTitle,currentNode->children->content);
      }
      if(strcmp(currentNode->name, "length")==0){
        svg_canvas.length = malloc(strlen(currentNode->children->content) +1);
        svg_canvas.length = currentNode->children->content;
      }

      if(strcmp(currentNode->name, "width")==0){
        svg_canvas.width = malloc(strlen(currentNode->children->content) +1);
        svg_canvas.width = currentNode->children->content;
      }

      if(strcmp(currentNode->name, "backcolor")==0){
        strcpy(svg_canvas.backcolor,currentNode->children->content);
      }

      if(strcmp(currentNode-> name, "name") == 0 /*&& strcmp(currentNode -> next-> name, "name")==0*/){
        if(strcmp(currentNode -> parent -> name , "Yaxis") == 0){
          svg_yaxis.name = malloc(strlen(currentNode->children->content) + 1);
          strcpy(svg_yaxis.name,currentNode->children->content);
        }
        if(strcmp(currentNode -> parent -> name , "Xaxis") == 0){
          svg_xaxis.name = malloc(strlen(currentNode->children->content) + 1);
          strcpy(svg_xaxis.name,currentNode->children->content);
        }
      }

      if(strcmp(currentNode->name, "forecolor")==0){
        strcpy(svg_xaxis.forecolor,currentNode->children->content);
      }

      if(strcmp(currentNode->name, "xdata") == 0){
        svg_xset.xData[xDataCounter] = malloc(strlen(currentNode->children->content) +1);
        strcpy(svg_xset.xData[xDataCounter], currentNode->children->content);
        xDataCounter++;
      }

      if(currentNode->properties != NULL){   //if the node has attribute
        for (NodeAttribute = currentNode->properties; NodeAttribute; NodeAttribute = NodeAttribute->next) {   //search all attributes
          attribute =  xmlNodeGetContent((xmlNode*)NodeAttribute);

          if(strcmp(NodeAttribute->name, "unit") == 0){
            svg_ysets[ySetCounter].unit = malloc(strlen((char*)attribute)+1);
            strcpy(svg_ysets[ySetCounter].unit,(char*)attribute);
          }

          if(strcmp(NodeAttribute->name, "name") == 0){
            svg_ysets[ySetCounter].name = malloc(strlen((char*)attribute)+1);
            strcpy(svg_ysets[ySetCounter].name,(char*)attribute);
          }
          if(strcmp(NodeAttribute->name, "showvalue") == 0){
            strcpy(svg_ysets[ySetCounter].showValue,(char*)attribute);
          }
          if(strcmp(NodeAttribute->name, "fillcolor") == 0){
            strcpy(svg_ysets[ySetCounter].fillColor,(char*)attribute);
          }
        }
      }

      if(strcmp(currentNode->name, "ydata") == 0){
        svg_ysets[ySetCounter].yData[yDataCounter] = malloc(strlen(currentNode->children->content)+1);
        strcpy(svg_ysets[ySetCounter].yData[yDataCounter],currentNode->children->content);
        yDataCounter++;
      }

      if(yDataCounter == 12){   // THIS PART IS NOT DYNAMIC
        yDataCounter == 0;
        ySetCounter++;
      }
    }
    xmlWalk(currentNode->children);
  }
}

void parseXML(char *xmlName){
  xmlDoc *doc = NULL;
  xmlNode *root_element = NULL;
  char *Filename = xmlName;
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

int main(int argc, char *argv[]) {
  int j = 0;

  for (j = 0; j < 10; j++) {

  }
  char *xmlName;
  char *xsdName;
  char *svgName;
  char **parsedInput = malloc(100);

  int i = 0;   //for's i

  for (i = 1; i < argc; i++) {
    if(strcmp(argv[1],"chartgen") == 0){
      if(strcmp(argv[i],"-i") == 0){
        parsing(parsedInput,argv[i+1],".");
        if(strcmp(parsedInput[1],"xml")==0){
          xmlName = malloc(strlen(argv[i+1]) + 10);
          strcpy(xmlName, argv[i+1]);
          xmlFlag = TRUE;
        }
      }

      if(strcmp(argv[i],"-v") == 0){
        parsing(parsedInput,argv[i+1],".");
        if(strcmp(parsedInput[1],"xsd")==0){
          xsdName = malloc(strlen(argv[i+1]) + 10);
          strcpy(xsdName, argv[i+1]);
          xsdFlag = TRUE;
        }
      }

      if(strcmp(argv[i],"-o") == 0){
        parsing(parsedInput,argv[i+1],".");
        if(strcmp(parsedInput[1],"svg") == 0){
          svgName = malloc(strlen(argv[i+1]) + 10);
          strcpy(svgName, argv[i+1]);
          SVGEntered = TRUE;
        }
      }

      if(strcmp(argv[i],"-t")==0){
        if(strcmp(argv[i+1],"line")==0){
          printf("%s\n", "NOT READY");
        }
        if(strcmp(argv[i+1],"pie")==0){
          circleSVG = 1;
        }
        if(strcmp(argv[i+1],"bar")==0){
          printf("%s\n", "NOT READY");
        }
      }

      if(strcmp(argv[i],"-h")==0){
        help();
      }
    }
  }

  //XML OPERATIONS
  if(xmlFlag == TRUE){
    parseXML(xmlName);
  }
  //XSD OPERATIONS
  if(xmlFlag == TRUE && xsdFlag == TRUE){
    xsdValidation(xsdName,xmlName);
    free(xsdName);
    free(xmlName);
  }
  //SVG OPERATIONS
  if(circleSVG == TRUE && SVGEntered == TRUE){
    createCircleSVG(svgName);
    free(svgName);
  }

  if(xmlFlag == FALSE || xsdFlag == FALSE){
    printf("%s\n", "DONT FORGET TO WRITE CHARTGEN AT THE BEGINNING");
    printf("%s\n", "There is some error xml or xsd process. Please check your input");
    printf("%s\n", "Check that you have given the XML in the code. XSD checks XML, if it isn't NULL\n");
    help();
  }
  if(SVGEntered == FALSE || circleSVG == FALSE){
    printf("%s\n", "DONT FORGET TO WRITE CHARTGEN AT THE BEGINNING");
    printf("%s\n", "Please give a output name with -o command");
    printf("%s\n", "If you given -o command, please check your input's -t part\n");
    help();
  }
  free(parsedInput);
  return 0;
}
