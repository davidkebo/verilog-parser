/* ---------------------------------------------------------------------------
** This software is in the public domain, furnished "as is", without technical
** support, and with no warranty, express or implied, as to its usefulness for
** any purpose.
**
** verilog-parser.c
** A verilog parser for multi-level combinational logic circuits
**
** Author: David Kebo Houngninou
** -------------------------------------------------------------------------*/
#ifdef IDENT_C
static const char* const <verilog_parser>_c_Id =
    "$Id$";
#endif


#include <sys/types.h>
#include <sys/time.h>
#include <stdio.h>
#include <string.h>
#include <time.h>
#include <math.h>
#include <stdlib.h>
#include "functions.h"

/**
 * Build a circuit from a verilog module object
 * @param a pointer to the verilog file, a verilog module object, a circuit object
 */
void build_module_circuit (FILE *verilog, module m, circuit c)
{
    int i=0, j=0, in=0, out=0, id=0, index=0;		/*Indexes*/
    char linebuf[LINESIZE], tmpbuf[LINESIZE], buffer[BUFSIZE];  /*Buffer for lines of the verilog code*/
    char *token[TOKENSIZE]; 					/*Array to hold tokens for the line*/

    memset(linebuf, 0, sizeof(char) * LINESIZE);
    memset(tmpbuf, 0, sizeof(char) * LINESIZE);
    memset(buffer, 0, sizeof(char) * BUFSIZE);
    memset(token, 0, sizeof(char*) * TOKENSIZE);

    /*Initialization of the circuit*/
    c->inputcount = m->inputcount;			/*Set the number of inputs for the ciruit*/
    c->outputcount = m->outputcount;		/*Set the number of outputs for the circuit*/
    c->wirecount = m->wirecount;				/*Set the number of wires for the circuit*/
    c->gatecount = m->gatecount;				/*Set the number of gates for the circuit*/
    c->size = c->inputcount+c->outputcount+c->wirecount+c->gatecount;/*Number of inputs, outputs and wires*/
    c->wires = (wire *)calloc((c->wirecount) * 10,sizeof(wire));	/*Allocate a contiguous array to index for every wire*/
    c->nodes = (node *)calloc((c->size) * 5,sizeof(node));		/*Allocate a contiguous array to index for every nodes*/

    for (i=0; i < c->inputcount; i++) {			/*Store the names of inputs*/
        c->inputs[i] = calloc (strlen(m->inputs [i]) + 1, sizeof (char));
        strcpy (c->inputs[i], m->inputs [i]);
        c->nodes[id] = calloc (1,sizeof(struct node_));
        setNode (c->nodes[id], "input", m->inputs [i], id); /*Set values of a node (type, name, id)*/
        id++;
    }
    for (i=0; i < c->outputcount; i++) {			/*Store the names of outputs*/
        c->outputs[i] = calloc (strlen(m->outputs [i]) + 1, sizeof(char));
        strcpy (c->outputs[i], m->outputs [i]);
        c->nodes[id] = calloc (1,sizeof(struct node_));
        setNode (c->nodes[id], "output", m->outputs [i], id); /*Set values of a node (type, name, id)*/
        id++;
    }
    for (i=0; i < c->wirecount; i++) {				/*Store the names of wires*/
        c->nodes[id] = calloc (1,sizeof(struct node_));
        setNode (c->nodes[id], "wire", m->wires [i], id); 	/*Set values of a node (type, name, id)*/
        id++;
    }
    for (i=0; i < c->gatecount; i++) {				/*Store the names of gates*/
        c->nodes[id] = calloc (1,sizeof(struct node_));
        setNode (c->nodes[id], "gate", m->gates [i], id);	/*Set values of a node (type, name, id)*/
        id++;
    }

    c->nodecount = id;

    while (fgets(linebuf,LINESIZE-1,verilog) != NULL) {	/*Create wires: gate_type [gate_name](out_port, in_port...)*/
        char *keyword = NULL;						/*Keyword from verilog line*/

        i=0;
        in=0;
        memset (buffer,0,sizeof(char) * BUFSIZE);				/*Clear the buffer*/
        strcpy (tmpbuf,linebuf);
        token[0] = strtok(tmpbuf, " [()],;"); 		/*Get 1st keyword from the line*/
        keyword = trim(token[0]);

        if (!reserved (keyword)) continue;		/*Skip any comment lines, empty lines or spaces*/
        if (end_of_module (linebuf)) break;		/*If end of module is reached then break*/
        if (!gate (keyword)) continue;			/*Skip everything but gates*/

        strcat (buffer,linebuf);

        while (!end_of_line(linebuf)) {			/*Check if the line ends with a ';' character (Multiple lines statement)*/
            if (fgets(linebuf,LINESIZE,verilog) != NULL)	/*Otherwise, append all the following lines until ';' is found*/
                strcat (buffer,linebuf);
        }

        token[0] = strtok(buffer, " [()],;"); 		/*Tokenize the line to extract data*/
        while(token[i]!= NULL) {
            i++;
            token[i] = strtok(NULL, " [()],;\r\n");
        }


        /*A. Create wires for all the gate inputs*/
        for(j = 2; j < i-1; j++) {
            if (!defined (c,token[j])) { /*If wire is not already defined*/
                c->wires[index] = (wire)calloc(1,sizeof(struct wire_));
                build_wire (c, c->wires[index], "I", token[j]);
                c->wires[index]->outputs[0] = getID (token[1], c);/*1/29/15 assign out to I wires*/
                c->wires[index]->outputcount = 1;
                index++;
            }
        }

        /*B. Create a wire for the gate*/
        c->wires[index] = (wire)calloc(1,sizeof(struct wire_));
        build_wire (c, c->wires[index], keyword, token[1]);
        /*B1. Assign the gate inputs*/
        in = 0;
        for(j = 2; j < i-1; j++) {
            c->wires[index]->inputs[in] = getID (token[j], c);
            c->wires[index]->inputcount++;
            in++;
        }
        /*B2. Assign the gate output*/
        c->wires[index]->outputs[0] = getID (token[i-1], c);
        c->wires[index]->outputcount = 1;
        index++;

        /*C. Create wires for the gate output*/
        if (!defined (c,token[i-1])) { /*If wire is not already defined*/
            c->wires[index] = (wire)calloc(1,sizeof(struct wire_));
            build_wire (c, c->wires[index], "I", token[i-1]);
            c->wires[index]->inputs[0] = getID (token[1], c);/*1/29/15 assign in to I wires*/
            c->wires[index]->inputcount = 1;
            index++;
        }

        else { /*If wire is already defined*/
            getWireByName(token[i-1],c)->inputs[0] = getID (token[1], c);/*Find the wire and attach an input to it (10/30/2015) */
            getWireByName(token[i-1],c)->inputcount = 1;
        }

        memset(linebuf, 0, sizeof(char) * LINESIZE);
        memset(tmpbuf, 0, sizeof(char) * LINESIZE);
        memset(buffer, 0, sizeof(char) * BUFSIZE);
        memset(token, 0, sizeof(char*) * TOKENSIZE);

    }//End while

    c->wirecount = index;

} /*End build_module_circuit*/



/**
 * Parse a signal vector
 * @param a collection of signals, a collection of tokens, a pointer to the index, a pointer to the number of counts
 */
void parse_signal_vector (char *signal_arr[], char *token[], int *index, int *count)
{
    int v,v1,v2;
    char *sig_vector; 					/*Array to hold tokens for the line*/
    sig_vector = strtok (token[*index],":");		/*Tokenize the vector to extract vector width*/
    v1 = atoi(sig_vector);  				/*Starting index for the vector*/
    sig_vector = strtok (NULL, ":");
    v2 = atoi(sig_vector);				/*Ending index for the vector*/
    (*index)++;						/*Get vector name from the next token*/
    for(v = v2; v <= v1; v++) {				/*Add the vector signals to the array of signals*/
        int wordsize = strlen(token[*index]);		/*Size of the string read from the line*/
        signal_arr [*count] = calloc (wordsize + 1, sizeof(char)); 	/*Allocating memory for signal string*/
        strcpy (signal_arr [*count], token[*index]); 	/*Add the signal name to the array of signals*/
        (*count)++;				 	/*Update the number of signals in the circuit*/
    }
}

/**
 * Parses a verilog file and extract modules statistical data
 * @param the name of the verilog file, the module object
 */
void build_module (FILE *verilog, module m)
{
    int i=0, j=0;						/*Indexes*/
    char linebuf[LINESIZE], tmpbuf[LINESIZE], buffer[BUFSIZE]; 	/*Buffer for lines of the verilog code*/
    char *token[TOKENSIZE]; 					/*Array to hold tokens for the line*/
    char *keyword;						/*Keyword from verilog line*/

    /*Make first pass to count primary inputs, primary outputs, and wires.*/
    while (fgets(linebuf,LINESIZE,verilog) != NULL) {
        i=0;
        strcpy (buffer,"");				/*Clear the buffer*/
        strcpy (tmpbuf,linebuf);
        token[0] = strtok(tmpbuf, " [()],;"); 		/*Get 1st keyword from the line*/
        keyword = trim(token[0]);
        if (!reserved (keyword)) continue;		/*skip comment lines, empty lines or spaces*/

        strcat (buffer,linebuf);
        while (!end_of_line(linebuf)) {			/*Check if the line ends with a ';' character (Multiple lines statement)*/
            if (end_of_module (linebuf)) break;		/*If end of module*/
            if (fgets(linebuf,LINESIZE,verilog) != NULL)	/*Otherwise, append all the following lines*/
                strcat (buffer,linebuf);
        }

        token[0] = strtok(buffer, " [()],;"); 		/*Tokenize the line to extract data*/
        while(token[i]!= NULL) {
            i++;
            token[i] = strtok(NULL, " [()],;\r\n");
        }

        if (strcmp(keyword, "module")==0) {		/*MODULES*/
            m->name = calloc (strlen(token[1]) + 1, sizeof(char)); 	/*Allocating memory for module name string*/
            strcpy (m->name,token[1]);			/*Set module name*/
        }

        else if (strcmp(keyword, "input")==0) {	/*INPUTS*/
            for(j = 1; j < i; j++) {			/*Parse all the words in the line*/
                if (signal_vector(token[j])) 		/*Handle a vector of signals*/
                    parse_signal_vector (m->inputs, token, &j, &m->inputcount);
                else {						 		/*Not a vector of signal*/
                    m->inputs [m->inputcount] = calloc (strlen(token[j]) + 1, sizeof(char)); 	/*Allocating memory for input string*/
                    strcpy (m->inputs [m->inputcount],token[j]);	 	/*Add the input name to the array of inputs*/
                    m->inputcount ++;				 		/*Update the number of inputs in the circuit*/
                }
            }
        }

        else if (strcmp(keyword, "output")==0) {	/*OUTPUTS*/
            for(j = 1; j < i; j++) {			/*Parse all the words in the line*/
                if (signal_vector(token[j]))  		/*Handle a vector of signals*/
                    parse_signal_vector (m->outputs, token, &j, &m->outputcount);
                else {						 	 	/*Not a vector of signal*/
                    m->outputs [m->outputcount] = calloc (strlen(token[j]) + 1, sizeof(char));	/*Allocating memory for output string*/
                    strcpy (m->outputs [m->outputcount],token[j]);		/*Add the output name to the array of outputs*/
                    m->outputcount ++;				 	 	/*Update the number of outputs in the circuit*/
                }
            }
        }

        else if (strcmp(keyword, "wire")==0) {		/*WIRES*/
            for(j = 1; j < i; j++) {			/*Parse all the words in the line*/
                if (signal_vector(token[j])) 		/*Handle a vector of signals*/
                    parse_signal_vector (m->wires, token, &j, &m->wirecount);
                else {						 		/*Not a vector of signal*/
                    m->wires [m->wirecount] = calloc (strlen(token[j]) + 1, sizeof(char)); 	/*Allocating memory for wire string*/
                    strcpy (m->wires [m->wirecount],token[j]);	 		/*Add the wire name to the array of wires*/
                    m->wirecount ++;				 		/*Update the number of wires in the circuit*/
                }
            }
        }

        else if (strcmp(keyword, "reg")==0) {		/*REGS*/
            for(j = 1; j < i; j++) {			/*Parse all the words in the line*/
                if (signal_vector(token[j])) 		/*Handle a vector of signals*/
                    parse_signal_vector (m->regs, token, &j, &m->regcount);
                else {						 	/*Not a vector of signal*/
                    m->regs [m->regcount] = calloc (strlen(token[j]) + 1, sizeof(char)); 	/*Allocating memory for reg string*/
                    strcpy (m->regs [m->regcount],token[j]);	 	/*Add the reg name to the array of regs*/
                    m->regcount ++;				 	/*Update the number of regs in the circuit*/
                }
            }
        }

        else if (gate (keyword)) {			/*GATES*/
            m->gates[m->gatecount] = calloc (strlen(token[1]) + 1, sizeof(char)); /*Allocating memory for module name string*/
            strcpy (m->gates [m->gatecount], token[1]);		/*Add the module name to the array of modules*/
            m->gatecount ++;					/*Update the number of modules in the circuit*/
        }

        else if (end_of_module (linebuf))		/*END OF MODULE*/
        {
            print_module_summary(m);			/*Print summary of the module*/
            break;					/*End of the module; break*/
        }
    } // end while

} /*end build_modules*/

/**
 * Parses a verilog file and create a circuit object using the module
 * @param the circuit object, the file name
 */
void parse_verilog_file (circuit c, char *filename)
{
    FILE *verilog; 			/* Verilog file */
    int i = 0;
    verilog = fopen(filename, "r"); 	/* Open Verilog file */
    if (!verilog) {
        fprintf(stderr,"transfer:  cannot open file \"%s\"\n",filename);
        exit(1);
    }

    module m = (module)calloc(1, sizeof(struct module_));	/*Declare an instance of a module*/
    build_module (verilog, m);			/*Create module object*/
    rewind(verilog);				/*Sets the stream position indicator to the beginning of verilog file.*/
    build_module_circuit (verilog, m, c); 	/*Create circuit object using the module*/

    /*Free Memory*/

    for (i = 0; i < m->outputcount; i++)
        free (m->outputs[i]);

    for (i = 0; i < m->inputcount; i++)
        free (m->inputs[i]);

    for (i = 0; i < m->wirecount; i++)
        free (m->wires[i]);

    for (i = 0; i < m->gatecount; i++)
        free (m->gates[i]);

    free (m->name);
    free (m); /*Free module memory*/

    fclose(verilog);

    /*End Free Memory*/
}

/**
 * Main program
 * @param argc, *argv[]
 */
int main (int argc, char *argv[])
{
    int i;
    if (argc != 2) {
        printf("Usage: ./verilog-parser <verilog_file.v>\n"); /* Check for a verilog input file */
        exit(1);
    }
    circuit c = (circuit)calloc(1,sizeof(struct circuit_));	/*Declare an instance of a circuit */

    c->name = strdup(argv[1]); 				/*Set circuit name*/

    parse_verilog_file (c, c->name);	 	/*Parse the verilog file */

    print_circuit_summary (c);			/*Print summary of the circuit */

    /*Free memory*/
    for (i=0; i < c->outputcount; i++)
        free (c->outputs[i]);

    for (i=0; i < c->inputcount; i++)
        free (c->inputs[i]);

    for (i=0; i < c->nodecount; i++)
        free (c->nodes[i]);
    free(c->nodes);

    for (i=0; i < c->wirecount; i++) {
        free (c->wires[i]->name);
        free (c->wires[i]->type);
        free (c->wires[i]);
    }
    free(c->wires);

    free (c->name);
    free (c); /*Deallocate memory used by the circuit*/

    return 0;
}
