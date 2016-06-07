/* ---------------------------------------------------------------------------
** This software is in the public domain, furnished "as is", without technical
** support, and with no warranty, express or implied, as to its usefulness for
** any purpose.
**
** functions.c
** A library of helper functions for the verilog parser
**
** Author: David Kebo Houngninou
** -------------------------------------------------------------------------*/
#ifdef IDENT_C
static const char* const functions_c_Id =
    "$Id$";
#endif

#include "keywords.h"
#include "functions.h"

/**
 * Determines if a string is reserved keyword
 * @param the string to compare
 * @return whether the string is reserved or not
 */
bool reserved (char* word)
{
    int i;
    for (i = 0; i < RESERVEDNUM; i++)
        if (strcmp(word, reserved_word[i])==0 || strstr(word, "endmodule")!= NULL)
            return true;
    return false;
}

/**
 * Determines if a string is gate
 * @param the string to check
 * @return whether the string is a gate or not
 */
bool gate (char* word)
{
    int i;
    for (i = 0; i < GATESNUM; i++)
        if (strcmp(word, gate_name[i])==0)
            return true;
    return false;
}

/**
 * Determines if a wire is an output
 * @param the string to check
 * @return whether the string is a an ouput or not
 */
bool isFinalOutput (wire w, circuit c)
{
    int i;
    for(i = 0; i < c->outputcount; i++)
        if (strcmp(w->name, c->outputs[i])==0)
            return true;
    return false;
}

/**
 * Parses a string and removes all character after a numeric character
 * @param the source string
 */
char* trim (char* source)
{
    int i=0, index=0;
    int sr_length = strlen(source);
    for(i=0; i<sr_length; i++)
    {
        if((source[i]=='x' && i!=0)) {
            source[index] = '\0';
            break;
        }
        else
            source[index++] = source[i];
    }
    source[index] = '\0';
    return source;
}

/**
 * Determines if a string is a vector of signals
 * @param the string to check
 * @return whether the string is a vector of signals or not
 */
bool signal_vector (char* word)
{
    int i;
    for (i = 0; i < strlen(word); i++)
        if (word[i] == ':')
            return true;
    return false;
}

/**
 * Determines if end of module
 * @param the string to check
 * @return if end of module or not
 */
bool end_of_module (char* word)
{
    if (strstr(word, "endmodule")!= NULL)
        return true;
    return false;
}

/**
 * Parses a string and search for the character ';'
 * @param the source string
 * @return whether the character ';' is found or not
 */
bool end_of_line(char* source)
{
    char * pchar = strchr(source, ';');
    return (pchar == NULL) ? false : true;
}

/**
 * Convert a gate name to an integer value
 * @param the string gate name
 * @return the gate integer value
 */
int convert (char* gate)
{
    if (strcasecmp(gate, "INPUT")==0)
        return 0;
    else if (strcasecmp(gate, "AND")==0)
        return 1;
    else if (strcasecmp(gate, "NAND")==0)
        return 2;
    else if (strcasecmp(gate, "OR")==0)
        return 3;
    else if (strcasecmp(gate, "NOR")==0)
        return 4;
    else if (strcasecmp(gate, "XOR")==0)
        return 5;
    else if (strcasecmp(gate, "XNOR")==0)
        return 6;
    else if (strcasecmp(gate, "BUF")==0)
        return 7;
    else if (strcasecmp(gate, "NOT")==0 || strcasecmp(gate, "INV")==0)
        return 8;
    else if (strcasecmp(gate, "I")==0)
        return 9;
    else
        return 10;
}

/**
 * Prints the summary of a module - Statistical information
 * @param the module object
 */
void print_module_summary (module m)
{
    int i;
    printf("\n************** Module %s statistical results *************\n", m->name);
    printf("Number of inputs: %d\n", m->inputcount);
    for(i = 0; i < m->inputcount; i++)
        printf("%s ", m->inputs[i]);

    printf("\n\nNumber of outputs: %d\n", m->outputcount);
    for(i = 0; i < m->outputcount; i++)
        printf("%s ", m->outputs[i]);

    printf("\n\nNumber of gates: %d\n", m->gatecount);
    for(i = 0; i < m->gatecount; i++)
        printf("%s ", m->gates[i]);

    printf("\n\nNumber of wires: %d\n", m->wirecount);
    for(i = 0; i < m->wirecount; i++)
        printf("%s ", m->wires[i]);

    printf("\n\nNumber of regs: %d\n", m->regcount);
    for(i = 0; i < m->regcount; i++)
        printf("%s ", m->regs[i]);
    printf("*************************** END **************************\n");
}

/**
 * Prints the summary of a circuit - Statistical information
 * @param the circuit object
 */
void print_circuit_summary (circuit c)
{
    int i,j,row,col;
    printf("\n************** Circuit %s statistical results *************\n", c->name);
    printf("Circuit size: %d\n", c->size);
    printf("Number of primary inputs: %d\n", c->inputcount);
    for(i = 0; i < c->inputcount; i++)
        printf("%s ", c->inputs[i]);

    printf("\n\nNumber of outputs: %d\n", c->outputcount);
    for(i = 0; i < c->outputcount; i++)
        printf("%s ", c->outputs[i]);

    printf("\n\nNumber of gates: %d\n", c->gatecount);

    printf("\n\n");
    i=0;

    while (i<c->wirecount && c->wires[i] != NULL) {
        printf ("c->wire[%d]->type: %s, ",i, c->wires[i]->type);
        printf ("ID: %d,  ", c->wires[i]->id);
        printf ("name: %s, ", c->wires[i]->name);

        printf ("\nInputs (%d): ", c->wires[i]->inputcount);/*Wire inputs*/
        for(j=0; j<c->wires[i]->inputcount; j++)
            printf ("%d ",c->wires[i]->inputs[j]);

        printf ("\nOutputs (%d): ", c->wires[i]->outputcount);/*Wire outputs*/
        for(j=0; j<c->wires[i]->outputcount; j++)
            printf ("%d ",c->wires[i]->outputs[j]);

        i++;
        printf ("\n");
    }
    printf("*************************** END **************************\n");
}

/**
 * Get the id of a wire
 * @param the signal name, the circuit's name
 * The id of the wire
 */
int getID (char* name, circuit c)
{
    int i;
    for(i=0; i<c->size; i++) {
        if (strcmp(name, c->nodes[i]->name)==0) { // If node is found in the circuit, get its index
            return c->nodes[i]->id;
        }
    }
    return 0;
}

/**
 * Get a wire by id
 * @param the signal name, the circuit's name
 */
wire getWire (int id, circuit c)
{
    int i=0;
    while (i<c->wirecount && c->wires[i] != NULL) {
        if (c->wires[i]->id == id) // If node is found in the circuit
            return c->wires[i];
        i++;
    }
    return 0;
}

/**
 * Get a wire by name
 * @param the signal name, the circuit's name
 */
wire getWireByName (char* name, circuit c)
{
    int i=0;
    while (c->wires[i] != NULL) {
        if (strcmp(name, c->wires[i]->name)==0) // If node is found in the circuit
            return c->wires[i];
        i++;
    }
    return 0;
}


/**
 * Set values of a node
 * @param the node object, the type of node, the name of the node, the node id
 */
void setNode ( node n, char* type, char* name, int id)
{
    strcpy(n->type, type);
    strcpy(n->name, name);
    n->id = id;			/*Store node id*/
}


/**
 * Create a wire
 * @param the circuit object, the wire object, the wire type, the wire name
 */
void build_wire (circuit c, wire w, char* type, char* name)
{
    int i;
    w->id = getID (name, c);   /*Wire ID*/
    w->type = strdup(type);		/*Wire type*/
    w->name = strdup(name);		/*Wire name*/
    w->inputcount = 0;			/*Initial number of inputs*/
    w->outputcount = 0;			/*Initial number of outputs*/

    for(i = 0; i < c->inputcount; i++) { /*Circuit primary inputs*/
        if (strcmp(w->name, c->inputs[i])==0) {
            w->primary = true;
        }
    }

    printf("Creating wire: %s, id: %d\n", w->name, w->id);
}

/**
 * Determines if a wire is already created
 * @param the circuit object, the wire name
 * @return whether the wire is already created or not
 */
bool defined (circuit c, char* name)
{
    int i=0;
    while (c->wires[i] != NULL) {
        if (strcmp(c->wires[i]->name, name)==0)
            return true;
        i++;
    }
    return false;
}
