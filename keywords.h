/* ---------------------------------------------------------------------------
** This software is in the public domain, furnished "as is", without technical
** support, and with no warranty, express or implied, as to its usefulness for
** any purpose.
**
** keywords.h
** Verilog keywords
**
** Author: David Kebo Houngninou
** -------------------------------------------------------------------------*/
#ifndef _keywords_h
#define _keywords_h

#ifdef IDENT_H
static const char* const keywords_h_Id =
    "$Id$";
#endif

#define	GATESNUM 18
char* gate_name[] = {
    "and",
    "nand",
    "or",
    "nor",
    "xor",
    "xnor",
    "buf",
    "bufif0",
    "bufif1",
    "cmos",
    "nmos",
    "nor",
    "not",
    "fo",
    "fo3",
    "fo4",
    "fi",
    "inv"
};

char* reserved_word[] = {	/*Reserved Verilog keywords*/
    "always",
    "and",
    "assign",
    "begin",
    "buf",
    "bufif0",
    "bufif1",
    "case",
    "casex",
    "casez",
    "cmos",
    "deassign",
    "default",
    "defparam",
    "disable",
    "edge",
    "else",
    "end",
    "endcase",
    "endfunction",
    "endmodule",
    "endprimitive",
    "endspecify",
    "endtable",
    "endtask",
    "event",
    "for",
    "for",
    "force",
    "forever",
    "function",
    "highz0",
    "highz1",
    "if",
    "ifnone",
    "initial",
    "inout",
    "input",
    "integer",
    "join",
    "large",
    "macromodule",
    "medium",
    "module",
    "nand",
    "negedge",
    "nmos",
    "nor",
    "not",
    "notif0",
    "notif1",
    "or",
    "output",
    "parameter",
    "pmos",
    "posedge",
    "primitive",
    "pull0",
    "pull1",
    "pulldown",
    "pullup",
    "rcmos",
    "real",
    "realtime",
    "reg",
    "release",
    "repeat",
    "rnmos",
    "rpmos",
    "rtran",
    "rtranif0",
    "rtranif1",
    "scalared",
    "small",
    "specify",
    "specparam",
    "strong0",
    "strong1",
    "supply0",
    "supply1",
    "table",
    "task",
    "time",
    "tran",
    "tranif0",
    "tranif1",
    "tri",
    "tri0",
    "tri1",
    "triand",
    "trior",
    "trireg",
    "vectored",
    "wait",
    "wand",
    "weak0",
    "weak1",
    "while",
    "wire",
    "wor",
    "xnor",
    "xor",
    "fo",
    "fo3",
    "fo4",
    "fi",
    "inv"
};

#define NUM_PUNCTUATIONS 4
char* punctuations[] = {
    ";",
    ".",
    " ",
    "\n",
};


#endif
