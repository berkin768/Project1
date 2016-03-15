//gcc project1.c -o project1.out -I/usr/include/libxml2 -lxml2 -lm
//./project1.out chartgen -v validation.xsd -i chartgendata.xml -o berkin.svg -t pie

#include "stdio.h"
#include "libxml/xmlschemastypes.h"
#include "libxml/parser.h"
#include "libxml/tree.h"
#include "split.h"
#include "math.h"

#define KBLU  "\x1B[34m"  //BLUE
#define KNRM  "\x1B[0m"   //BLACK
#define TRUE 1            //BOOLEAN
#define FALSE 0           //BOOLEAN

char *chartTitle; //chartTitle keeps Monthly Sales for example

struct canvas{
  char *length;
  char *width;
  char *backcolor;
};

struct Yaxis{
  char *name;
};

struct Xaxis{
  char *name;
  char *forecolor;
};

struct Xset{
  char *xData[100];
};

struct Yset{
  char *unit;
  char *name;
  char *showValue;
  char *fillColor;
  char *yData[100];
};

struct canvas svg_canvas;    //svg_canvas.length  svg_canvas.width  svg_canvas.backcolor
struct Yaxis svg_yaxis;      //svg_yaxis.name
struct Xaxis svg_xaxis;      //svg_xaxis.name  svg_xaxis.forecolor
struct Xset svg_xset;        //svg_xset.xData[FOR STRUCTURE]
struct Yset svg_ysets[50];   //svg_ysets.unit  svg_ysets.name  svg_ysets.showValue  svg_ysets.fillColor  svg_ysets.yData[FOR STRUCTURE]

int xmlFlag = FALSE;         //is XML readed?
int xsdFlag = FALSE;         //is XSD readed?
int SVGEntered = FALSE;      //is SVG readed?  -o command
int circleSVG = FALSE;       //is Type given?  -t pie
int barSVG = FALSE;          //is Type given?  -t bar
int lineSVG = FALSE;         //is Type given?  -t line

int ySetCounter = 0;         //ySet counts set of Y. For example chartgendata.xml has 2 Y set, so this variable = 2
int yDataCounter = 0;        //This variable to control number of Ydata. It return 0 after Yset done. *DONT USE IT IN FOR STRUCTURE!*
int tempYDataCounter = 0;    //This variable keeps number of YData quantity. It's 6 on the chartgendata.xml
int xDataCounter = 0;        //This variable keeps number of XData quantity. It's 6 on the chartgendata.xml. YOU CAN USE IN FOR STRUCTURE
int tempXDataCounter = 1;    //This variable compare Ydata quantity and Xdata quantity in the XMLWalk function. *DONT USE IT IN FOR STRUCTURE!*


xmlDocPtr pieSVG = NULL; //MY SVG FILE, CREATE YOUR OWN
xmlNodePtr body = NULL;  //BODY IS ROOT
xmlNodePtr canvas = NULL, charttitle = NULL, xset = NULL, yset = NULL, path = NULL, colors = NULL, cities = NULL; //SVG PARTS

void boxColors(xmlNodePtr colors,int j){  //COLORS
  if(j == 0)
  xmlNewProp(colors, BAD_CAST "fill", BAD_CAST "lightcoral");
  if(j == 1)
  xmlNewProp(colors, BAD_CAST "fill", BAD_CAST "lightseagreen");
  if(j == 2)
  xmlNewProp(colors, BAD_CAST "fill", BAD_CAST "rosybrown");
  if(j == 3)
  xmlNewProp(colors, BAD_CAST "fill", BAD_CAST "darkgreen");
  if(j == 4)
  xmlNewProp(colors, BAD_CAST "fill", BAD_CAST "slategrey");
  if(j == 5)
  xmlNewProp(colors, BAD_CAST "fill", BAD_CAST "sienna");
  if(j == 6)
  xmlNewProp(colors, BAD_CAST "fill", BAD_CAST "brown");
  if(j == 7)
  xmlNewProp(colors, BAD_CAST "fill", BAD_CAST "maroon");
  if(j == 8)
  xmlNewProp(colors, BAD_CAST "fill", BAD_CAST "orangered");
  if(j == 9)
  xmlNewProp(colors, BAD_CAST "fill", BAD_CAST "dodgerblue");
  if(j == 10)
  xmlNewProp(colors, BAD_CAST "fill", BAD_CAST "chocolate");
  if(j == 11)
  xmlNewProp(colors, BAD_CAST "fill", BAD_CAST "midnightblue");
  if(j == 12)
  xmlNewProp(colors, BAD_CAST "fill", BAD_CAST "salmon");
  if(j == 13)
  xmlNewProp(colors, BAD_CAST "fill", BAD_CAST "gold");
}

void createPieSVG(char *svgName){
  double totalValue = 0;    //Total value of Ydata on each Yset, using for calculating angle
  double elementAngle = 0;  //Angle of each Ydata
  double startAngle = 0;    //Starting position of each pie corner
  double endAngle = 0;      //Ending position of each pie corner
  char *pathText;           //Path attribute
  char *height = malloc(5); //Canvas's height | width
  char *cy = malloc(5);     //Path's middle point on Y axis.
  char *textPosition = malloc(5);     //This variable sets yData and xData's position
  char *boxPosition = malloc(5);      //This variable sets colored boxes's position
  char *cityPosition = malloc(5);     //This variable sets city name's position
  char *chartTitlePosition = malloc(5);  //This variable sets charttitle's position
  int i = 0;                //ySetCounter
  int j = 0;                //xDataCounter == tempYDataCounter
  int k = 0;                //xDataCounter == tempYDataCounter -- DOES DIFFERENT THINGS
  int firstcy = 0, lastcy = 0;        //Path's middle coordinates. They created because i wanted to make charttitle on the middle

  pieSVG = xmlNewDoc(BAD_CAST "1.0");    //Creating SVG

  body = xmlNewNode(NULL, BAD_CAST"body");    //Creating new node, our root
  xmlDocSetRootElement(pieSVG, body);        //Root assigned

  //-------------------CANVAS PART------------------
  sprintf(height,"%d",(atoi(svg_canvas.length) * (ySetCounter+1)));  //Dynamic canvas size

  canvas = xmlNewChild(body, NULL, BAD_CAST"svg", NULL);             //Child of body (root)
  xmlNewProp(canvas,BAD_CAST "xmlns","http://www.w3.org/2000/svg");             //I dont know :D
  xmlNewProp(canvas, BAD_CAST "height", height);             //Dynamic height
  xmlNewProp(canvas, BAD_CAST "width", height);             //Dynamic width
  //--------------------DONE-----------------------

  for (i = 0; i < ySetCounter; i++) {   //How many circle?
    totalValue = 0;                           //Total value of yData sets 0 for another ySet

    //-------------------CITY PART------------------
    sprintf(cityPosition, "%d", (30+i*300));  //Dynamic city name position
    cities = xmlNewChild(canvas, NULL, BAD_CAST "text", svg_ysets[i].name);   //City names
    xmlNewProp(cities, BAD_CAST "x", "310");                                  //X Coordinate = 310
    xmlNewProp(cities, BAD_CAST "y", cityPosition);                           //Y Coordinate = DYNAMIC
    xmlNewProp(cities, BAD_CAST "font-family", BAD_CAST "Verdana");           //Font type
    xmlNewProp(cities, BAD_CAST "font-size", BAD_CAST "25");                  //Font size
    xmlNewProp(cities, BAD_CAST "fill", BAD_CAST "teal");                     //Font color
    //--------------------DONE-----------------------

    for (j = 0; j < xDataCounter; j++) {  //PRINT MONTHS & VALUES & COLORS - XDATA & YDATA & COLORS

      //-------------------XDATA PART------------------
      sprintf(textPosition, "%d", ((60+j*20)+i*300));                       //Dynamic xData position
      xset = xmlNewChild(canvas, NULL, BAD_CAST "text", svg_xset.xData[j]); //xData values
      xmlNewProp(xset, BAD_CAST "x", "400");                                //X Coordinate = 400
      xmlNewProp(xset, BAD_CAST "y", textPosition);                         //Y Coordinate = DYNAMIC
      xmlNewProp(xset, BAD_CAST "font-family", BAD_CAST "Verdana");         //Font type
      //--------------------DONE-----------------------

      //-------------------YDATA PART------------------
      if(svg_ysets[i].showValue != NULL && strcmp(svg_ysets[i].showValue,"yes") == 0){  //if showvalue is equal yes
        yset = xmlNewChild(canvas, NULL, BAD_CAST "text", svg_ysets[i].yData[j]);       //YData values
        xmlNewProp(yset, BAD_CAST "x", "250");                                          //X Coordinate = 250
        xmlNewProp(yset, BAD_CAST "y", textPosition);                                   //Y Coordinate = DYNAMIC
        xmlNewProp(yset, BAD_CAST "font-family", BAD_CAST "Verdana");                   //Font type
      }
      //--------------------DONE-----------------------

      //-------------------BOX PART------------------
      sprintf(boxPosition, "%d", ((50+j*20)+i*300));  //dynamic y coordinate
      colors = xmlNewChild(canvas, NULL, BAD_CAST "rect", NULL);
      xmlNewProp(colors, BAD_CAST "x", "325");
      xmlNewProp(colors, BAD_CAST "y", boxPosition);
      xmlNewProp(colors, BAD_CAST "width", "40");
      xmlNewProp(colors, BAD_CAST "height", "15");
      boxColors(colors,j);
      //--------------------DONE-----------------------


      totalValue = totalValue + atoi(svg_ysets[i].yData[j]);  //Total value of ySet
    }

    //-------------------PIE PART------------------
    for (k = 0; k < xDataCounter; k++) {        //ySet or xSet number
      sprintf(cy, "%d", (100+i*300));           //Dynamic path's center
      elementAngle = (atoi(svg_ysets[i].yData[k]) * 360) / totalValue;     //Angles for eich one yData
      startAngle = endAngle;                                               //Pie's starting corner
      endAngle = startAngle + elementAngle;                                //Pie's ending corner
      int x1 = (125 + 90* cos(M_PI*startAngle/180));                       //slices
      int y1 = ((100+i*300) + 90* sin(M_PI*startAngle/180));
      int x2 = (125 + 90* cos(M_PI*endAngle/180));
      int y2 = ((100+i*300) + 90* sin(M_PI*endAngle/180));
      pathText = malloc(256);

      if(elementAngle < 180)
      sprintf(pathText, "M 125,%s  L %d,%d A 90,90 0 0,1 %d,%d z",cy,x1, y1, x2, y2);
      else
      sprintf(pathText, "M 125,%s  L %d,%d A 90,90 0 1,1 %d,%d z",cy,x1, y1, x2, y2);

      path=xmlNewChild(canvas, NULL, BAD_CAST "path", NULL);

      xmlNewProp(path, BAD_CAST "d", BAD_CAST pathText);
      boxColors(path,k);
    }
    //--------------------DONE-----------------------


    //TWO CENTER OF PATH'S MIDDLE FOR CHATTITLE
    if(i == 0)
    firstcy = atoi(cy);
    if(i == ySetCounter-1)
    lastcy = atoi(cy);

  }

  //-------------------CHARTTITLE PART------------------

  sprintf(chartTitlePosition, "%d", (lastcy+firstcy) /3);  //dynamic y coordinate
  charttitle = xmlNewChild(canvas, NULL, BAD_CAST "text", chartTitle);
  xmlNewProp(charttitle, BAD_CAST "x", BAD_CAST chartTitlePosition);
  xmlNewProp(charttitle, BAD_CAST "y", BAD_CAST"45");
  xmlNewProp(charttitle, BAD_CAST "transform", BAD_CAST "rotate(90 20,40)");
  xmlNewProp(charttitle, BAD_CAST "font-family", BAD_CAST "Verdana");

  //--------------------DONE-----------------------

  htmlSaveFileEnc(svgName, pieSVG, "UTF-­8", 1);

  xmlFreeDoc(pieSVG);
  free(height);
  free(cy);
  free(textPosition);
  free(boxPosition);
  free(cityPosition);
  free(chartTitlePosition);
  free(height);
}

void createLineSVG(char *svgName){
  //TODO Line SVG

  /*  //SVG's DOCUMENT STARTS HERE
  svg = xmlNewDoc(BAD_CAST "1.0");

  body = xmlNewNode(NULL, BAD_CAST"body");
  xmlDocSetRootElement(svg, body);
  */


  /*
  htmlSaveFileEnc(svgName, svg, "UTF-­8", 1);

  xmlFreeDoc(svg);
  */
}

void createBarSVG(char *svgName){
  //TODO Bar SVG

  /*  //SVG's DOCUMENT STARTS HERE
  svg = xmlNewDoc(BAD_CAST "1.0");

  body = xmlNewNode(NULL, BAD_CAST"body");
  xmlDocSetRootElement(svg, body);
  */


  /*
  htmlSaveFileEnc(svgName, svg, "UTF-­8", 1);

  xmlFreeDoc(svg);
  */
}

void xsdValidation(char *xsdName, char *xmlName){
  char xmlLint[30];
  strcpy(xmlLint,"xmllint --noout --schema "); //TERMINAL COMMAND. IT REQURIES  [sudo] apt-get install libxml2-utils

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
        svg_canvas.backcolor = malloc(strlen(currentNode->children->content) + 1);
        strcpy(svg_canvas.backcolor,currentNode->children->content);
      }

      if(strcmp(currentNode-> name, "name")==0){
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
        svg_xaxis.forecolor = malloc(strlen(currentNode->children->content) + 1);
        strcpy(svg_xaxis.forecolor,currentNode->children->content);
      }

      if(strcmp(currentNode->name, "xdata") == 0){
        svg_xset.xData[xDataCounter] = malloc(strlen(currentNode->children->content) +1);
        strcpy(svg_xset.xData[xDataCounter], currentNode->children->content);
        xDataCounter++;
      }

      if(currentNode->properties != NULL){   //if the node has attribute
        tempXDataCounter = xDataCounter;
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
            svg_ysets[ySetCounter].showValue = malloc(strlen((char*)attribute)+1);
            strcpy(svg_ysets[ySetCounter].showValue,(char*)attribute);
          }
          if(strcmp(NodeAttribute->name, "fillcolor") == 0){
            svg_ysets[ySetCounter].fillColor = malloc(strlen((char*)attribute)+1);
            strcpy(svg_ysets[ySetCounter].fillColor,(char*)attribute);
          }
        }
      }


      if(strcmp(currentNode->name, "ydata") == 0){
        svg_ysets[ySetCounter].yData[yDataCounter] = malloc(strlen(currentNode->children->content)+1);
        strcpy(svg_ysets[ySetCounter].yData[yDataCounter],currentNode->children->content);
        yDataCounter++;
      }

      if(yDataCounter == tempXDataCounter){   // THIS PART IS PARTLY NOT DYNAMIC -BASED ON X NUMBER
        tempYDataCounter = yDataCounter;
        yDataCounter = 0;
        ySetCounter = ySetCounter +1;
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
  printf("%s\n","4- 'chartgen -t <type>' they are 'pie' 'bar' 'line'");
}

int main(int argc, char *argv[]) {
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
          lineSVG = 1;
        }
        if(strcmp(argv[i+1],"pie")==0){
          circleSVG = 1;
        }
        if(strcmp(argv[i+1],"bar")==0){
          barSVG = 1;
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
    createPieSVG(svgName);
    free(svgName);
  }

  //TODO Add your codes below

  /*if(lineSVG == TRUE && SVGEntered == TRUE){
    createLineSVG(svgName);
    free(svgName);
  }*/

  /*if(barSVG == TRUE && SVGEntered == TRUE){
    createBarSVG(svgName);
    free(svgName);
  }*/

  if(xmlFlag == FALSE || xsdFlag == FALSE){
    printf("%s\n", "DONT FORGET TO WRITE CHARTGEN AT THE BEGINNING");
    printf("%s\n", "There is some error xml or xsd process. Please check your input");
    printf("%s\n", "Check that you have given the XML in the code. XSD checks XML, if it isn't NULL\n");
    help();
  }
  if(SVGEntered == FALSE || (circleSVG == FALSE && barSVG == FALSE && lineSVG == FALSE)){
    printf("%s\n", "DONT FORGET TO WRITE CHARTGEN AT THE BEGINNING");
    printf("%s\n", "Please give a output name with -o command");
    printf("%s\n", "If you given -o command, please check your input's -t part\n");
    help();
  }
  free(parsedInput);
  return 0;
}
