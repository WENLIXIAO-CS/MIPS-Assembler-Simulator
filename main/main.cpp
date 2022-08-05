# define HI 32
# define LO 33
# define MEM_START 4194304

# include <map>
# include <bitset>
# include <cstdio>
# include <string>
# include <vector>
# include <fstream>
# include <sstream>
# include <iterator>
# include <iostream>

#include<fcntl.h>
#include<unistd.h>
#include <stdio.h>
using namespace std;


struct code_type{
    char c_type;
    string f_order; // rs-> s | rt-> t | rd-> d | shamt-> h | immdediate-> i | address-> a | label-> l
    string op, shamt, funct;
    string rs, rt, rd;
    string immediate, address;
    code_type(){}
    code_type(char c_type, string f_order, string op, string rs, string rt, string rd, string shamt, string funct){
      this->c_type = c_type;
      this->f_order = f_order;
      this->op = op;
      this->rs = rs;
      this->rt = rt;
      this->rd = rd;
      this->shamt = shamt;
      this->funct = funct;
    }
    code_type(char c_type, string f_order, string op, string rs, string rt, string immediate){
        this->c_type = c_type;
        this->f_order = f_order;
        this->op = op;
        this->rs = rs;
        this->rt = rt;
        this->immediate = immediate;
    }
    code_type(char c_type, string f_order, string op, string address){
        this->c_type = c_type;
        this->f_order = f_order;

        this->op = op;
        this->address = address;
    }
};

map<string, code_type> InstMap;                     // define instruction-type map
map<string, string> RegMap;  // define register-bin map

ifstream inFile;
ofstream outFile;

void init_InstMap(){
  InstMap["add"] = code_type('R',"dst","000000","_____","_____","_____","00000","100000");
  InstMap["addu"] = code_type('R',"dst","000000","_____","_____","_____","00000","100001");
  InstMap["addi"] = code_type('I',"tsi","001000","_____","_____","________________");
  InstMap["addiu"] = code_type('I',"tsi","001001","_____","_____","________________");
  InstMap["and"] = code_type('R',"dst","000000","_____","_____","_____","00000","100100");
  InstMap["andi"] = code_type('I',"tsi","001100","_____","_____","________________");
  InstMap["clo"] = code_type('R',"ds","011100","_____","00000","_____","00000","100001");
  InstMap["clz"] = code_type('R',"ds","011100","_____","00000","_____","00000","100000");
  InstMap["div"] = code_type('R',"st","000000","_____","_____","00000","00000","011010");
  InstMap["divu"] = code_type('R',"st","000000","_____","_____","00000","00000","011011");
  InstMap["mult"] = code_type('R',"st","000000","_____","_____","00000","00000","011000");
  InstMap["multu"] = code_type('R',"st","000000","_____","_____","00000","00000","011001");
  InstMap["mul"] = code_type('R',"dst","011100","_____","_____","_____","00000","000010");
  InstMap["madd"] = code_type('R',"st","011100","_____","_____","00000","00000","000000");
  InstMap["msub"] = code_type('R',"st","011100","_____","_____","00000","00000","000100");
  InstMap["maddu"] = code_type('R',"st","011100","_____","_____","00000","00000","000001");
  InstMap["msubu"] = code_type('R',"st","011100","_____","_____","00000","00000","000101");
  InstMap["nor"] = code_type('R',"dst","000000","_____","_____","_____","00000","100111");
  InstMap["or"] = code_type('R',"dst","000000","_____","_____","_____","00000","100101");
  InstMap["ori"] = code_type('I',"tsi","001101","_____","_____","________________");
  InstMap["sll"] = code_type('R',"dth","000000","00000","_____","_____","_____","000000");
  InstMap["sllv"] = code_type('R',"dts","000000","_____","_____","_____","00000","000100");
  InstMap["sra"] = code_type('R',"dth","000000","00000","_____","_____","_____","000011");
  InstMap["srav"] = code_type('R',"dts","000000","_____","_____","_____","00000","000111");
  InstMap["srl"] = code_type('R',"dth","000000","00000","_____","_____","_____","000010");
  InstMap["srlv"] = code_type('R',"dts","000000","_____","_____","_____","00000","000110");
  InstMap["sub"] = code_type('R',"dst","000000","_____","_____","_____","00000","100010");
  InstMap["subu"] = code_type('R',"dst","000000","_____","_____","_____","00000","100011");
  InstMap["xor"] = code_type('R',"dst","000000","_____","_____","_____","00000","100110");
  InstMap["xori"] = code_type('I',"tsi","001110","_____","_____","________________");
  InstMap["lui"] = code_type('I',"ti","001111","00000","_____","________________");
  InstMap["slt"] = code_type('R',"dst","000000","_____","_____","_____","00000","101010");
  InstMap["sltu"] = code_type('R',"dst","000000","_____","_____","_____","00000","101011");
  InstMap["slti"] = code_type('I',"tsi","001010","_____","_____","________________");
  InstMap["sltiu"] = code_type('I',"tsi","001011","_____","_____","________________");
  InstMap["beq"] = code_type('I',"stl","000100","_____","_____","________________");
  InstMap["bgez"] = code_type('I',"sl","000001","_____","00001","________________");
  InstMap["bgezal"] = code_type('I',"sl","000001","_____","10001","________________");
  InstMap["bgtz"] = code_type('I',"sl","000111","_____","00000","________________");
  InstMap["blez"] = code_type('I',"sl","000110","_____","00000","________________");
  InstMap["bltzal"] = code_type('I',"sl","000001","_____","10000","________________");
  InstMap["bltz"] = code_type('I',"sl","000001","_____","00000","________________");
  InstMap["bne"] = code_type('I',"stl","000101","_____","_____","________________");
  InstMap["j"] = code_type('J',"a","000010","__________________________");
  InstMap["jal"] = code_type('J',"a","000011","__________________________");
  InstMap["jalr"] = code_type('R',"sd","000000","_____","00000","_____","00000","001001");
  InstMap["jr"] = code_type('R',"s","000000","_____","00000","00000","00000","001000");
  InstMap["teq"] = code_type('R',"st","000000","_____","_____","00000","00000","110100");
  InstMap["teqi"] = code_type('I',"si","000001","_____","01100","________________");
  InstMap["tne"] = code_type('R',"st","000000","_____","_____","00000","00000","110110");
  InstMap["tnei"] = code_type('I',"si","000001","_____","01110","________________");
  InstMap["tge"] = code_type('R',"st","000000","_____","_____","00000","00000","110000");
  InstMap["tgeu"] = code_type('R',"st","000000","_____","_____","00000","00000","110001");
  InstMap["tgei"] = code_type('I',"si","000001","_____","01000","________________");
  InstMap["tgeiu"] = code_type('I',"si","000001","_____","01001","________________");
  InstMap["tlt"] = code_type('R',"st","000000","_____","_____","00000","00000","110010");
  InstMap["tltu"] = code_type('R',"st","000000","_____","_____","00000","00000","110011");
  InstMap["tlti"] = code_type('I',"si","000001","_____","01010","________________");
  InstMap["tltiu"] = code_type('I',"si","000001","_____","01011","________________");
  InstMap["lb"] = code_type('I',"tis","100000","_____","_____","________________");
  InstMap["lbu"] = code_type('I',"tis","100100","_____","_____","________________");
  InstMap["lh"] = code_type('I',"tis","100001","_____","_____","________________");
  InstMap["lhu"] = code_type('I',"tis","100101","_____","_____","________________");
  InstMap["lw"] = code_type('I',"tis","100011","_____","_____","________________");
  InstMap["lwl"] = code_type('I',"tis","100010","_____","_____","________________");
  InstMap["lwr"] = code_type('I',"tis","100110","_____","_____","________________");
  InstMap["ll"] = code_type('I',"tis","110000","_____","_____","________________");
  InstMap["sb"] = code_type('I',"tis","101000","_____","_____","________________");
  InstMap["sh"] = code_type('I',"tis","101001","_____","_____","________________");
  InstMap["sw"] = code_type('I',"tis","101011","_____","_____","________________");
  InstMap["swl"] = code_type('I',"tis","101010","_____","_____","________________");
  InstMap["swr"] = code_type('I',"tis","101110","_____","_____","________________");
  InstMap["sc"] = code_type('I',"tis","111000","_____","_____","________________");
  InstMap["mfhi"] = code_type('R',"d","000000","00000","00000","_____","00000","010000");
  InstMap["mflo"] = code_type('R',"d","000000","00000","00000","_____","00000","010010");
  InstMap["mthi"] = code_type('R',"s","000000","_____","00000","00000","00000","010001");
  InstMap["mtlo"] = code_type('R',"s","000000","_____","00000","00000","00000","010011");
  InstMap["syscall"] = code_type('R',"_","000000","00000","00000","00000","00000","001100");                // init inst map
}

void init_RegMap(){
  RegMap["$zero"] = "00000";
  RegMap["$at"] = "00001";
  RegMap["$v0"] = "00010";
  RegMap["$v1"] = "00011";
  RegMap["$a0"] = "00100";
  RegMap["$a1"] = "00101";
  RegMap["$a2"] = "00110";
  RegMap["$a3"] = "00111";
  RegMap["$t0"] = "01000";
  RegMap["$t1"] = "01001";
  RegMap["$t2"] = "01010";
  RegMap["$t3"] = "01011";
  RegMap["$t4"] = "01100";
  RegMap["$t5"] = "01101";
  RegMap["$t6"] = "01110";
  RegMap["$t7"] = "01111";
  RegMap["$s0"] = "10000";
  RegMap["$s1"] = "10001";
  RegMap["$s2"] = "10010";
  RegMap["$s3"] = "10011";
  RegMap["$s4"] = "10100";
  RegMap["$s5"] = "10101";
  RegMap["$s6"] = "10110";
  RegMap["$s7"] = "10111";
  RegMap["$t8"] = "11000";
  RegMap["$t9"] = "11001";
  RegMap["$k0"] = "11010";
  RegMap["$k1"] = "11011";
  RegMap["$gp"] = "11100";
  RegMap["$sp"] = "11101";
  RegMap["$fp"] = "11110";
  RegMap["$ra"] = "11111";                 // init
}

void init_args(){
  init_InstMap();
  init_RegMap();
}

map<string, int> label_addr;
vector< vector<string> > inst_list;

vector<string> split_line(string line){                                 // split the instruction line
  int pl = 0, pr;
  while (line[pl] == ' '
        || line[pl] == '\t'
        || line[pl] == '('
        || line[pl] == ')'
        || line[pl] == ',')
    ++ pl;                                                              // initialize pl
  pr = pl + 1;

  vector<string> line_elements;

  while (pl < line.length() && line[pl] != '#' && line[pl] != '\n'){
    while (pr != line.length()
          && line[pr] != ' '
          && line[pr] != '\t'
          && line[pr] != ','
          && line[pr] != '('
          && line[pr] != ')'
          && line[pr] != '#')
      ++ pr;
    line_elements.push_back(line.substr(pl, pr-pl));
    pl = pr;
    while (line[pl] == ' '
            || line[pl] == '\t'
            || line[pl] == ','
            || line[pl] == '('
            || line[pl] == ')')
      ++ pl;
    pr = pl + 1;
  }

  return line_elements;
}

string deal_with_string(string str){        // \t, \n, ...
    vector<char> ret;
    for (int i = 0; i < str.length(); ++ i){
        if (str[i] == '\\'){
            switch(str[i+1]){
                case '\\':{
                    ret.push_back('\\');
                    break;
                }
                case 'n':{
                    ret.push_back('\n');
                    break;
                }
                case 't':{
                    ret.push_back('\t');
                    break;
                }
                case '\'':{
                    ret.push_back('\'');
                    break;
                }
                case '\"':{
                    ret.push_back('\"');
                    break;
                }
                case '0':{
                    ret.push_back('\0');
                    break;
            }
            }
            i++;
        } else
            ret.push_back(str[i]);
    }
    string s_ret = string(ret.size(), '0');
    for (int i = 0; i < ret.size(); ++ i)
    s_ret[i] = ret[i];
    return s_ret;
}

map<string, string> extract_text_seg(string f_str){                                  // extract .text and .data from MIPS file
    for (int i = 0; i < f_str.length(); ++ i)
      if (f_str[i] == '#')
          for (int j = i+1; f_str[j] != '\n'; ++ j)
              f_str[j] = ' ', i = j;

    int n_chr = f_str.length(),
        p_data = f_str.find(".data"), p_text = f_str.find(".text");

    map<string, string> ret;

    if (p_text < p_data){
        ret["text"] = p_text == -1 ? "" : f_str.substr(p_text+5, p_data-p_text-5);
        ret["data"] = f_str.substr(p_data+5, n_chr-p_data-5);
    } else if (p_text > p_data){
        ret["text"] = f_str.substr(p_text+5, n_chr-p_text-5);
        ret["data"] = p_data == -1 ? "" : f_str.substr(p_data+5, p_text-p_data-5);
    } else {
      ret["text"] = "", ret["data"] = "";
    }

    return ret;
}

vector<string> sep_lines(string text_content){
  vector<string> ret;
  for (int i = 0; i < text_content.length(); ){
    int j = i;
    while (j!= text_content.length() && text_content[j] != '\n') ++ j;
    ret.push_back(text_content.substr(i, j-i));
    i = j+1;
  }
  return ret;
}

void sign_label_addr(string text_content){
  string str = text_content;                                              // remaining text

  int ptr_inst = 0;

  vector<string> label_buff, line_list;
  line_list = sep_lines(text_content);
  for (int ptr = 0; ptr < line_list.size(); ++ ptr) {

    string line = line_list[ptr];
    vector<string> line_e = split_line(line);

    if (line_e.size() == 0) continue;


    string label = line_e[0];
    if (label.back() == ':'){
      label.pop_back();
      label_buff.push_back(label);

      // pop front
      vector<string> tmp;
      for(int tmp_i=1; tmp_i<line_e.size(); ++ tmp_i){
        tmp.push_back(line_e[tmp_i]);
      }
      while (line_e.size() != 0) line_e.pop_back();
      for (int tmp_i=0; tmp_i<tmp.size(); ++ tmp_i)
        line_e.push_back(tmp[tmp_i]);
  //    line_e.erase(0);
    }

    if (line_e.size() == 0) continue;

    while (label_buff.size() != 0){
      string label_i = label_buff.back();
      label_buff.pop_back();
      label_addr[label_i] = ptr_inst;
    }
  //  cout << "hh" << " " << line_e.size() << endl;
    inst_list.push_back(line_e);

    ptr_inst ++;
  }
}

string extend_to_bin(int num, int len){
  string str = bitset<32>(num).to_string();
  string str_norm = str.substr(str.length()-len, len);
  return str_norm;
}

string extend_to_bin(string x, int len){                          // convert a num to bin and normalize length
  int num = stoi(x);
  return extend_to_bin(num, len);
}

string extend_to_bin(unsigned num, int len){
  string str = bitset<32>(num).to_string();
  string str_norm = str.substr(str.length()-len, len);
  return str_norm;
}

int number_to_dec(string x){                                      // convert hex / dec number -> dec
  string str;
  int ret;
  if (x[1] == 'x'){   // hex
    str = x.substr(2, x.length()-2);
    ret = stoi(str, nullptr, 16);
  } else{
    str = x;
    ret = stoi(str, nullptr, 10);
  }
  return ret;
}

//int PC;
int heapHead;
uint32_t *mem = new uint32_t[6 << 18];
uint32_t *reg = new uint32_t[34];  // 32 33 hi lo

uint32_t bit_2_uint(string str){                               // uint32_t convert bin -> decimal
  return stoul(str, nullptr, 2);
}

int bit_2_int(string str){                                        // int convert bin -> decimal
  return stoi(str, nullptr, 2);
}

int bit_2_int_signed(string str){
  string ops = str;
  int ret = 1;
  if (ops[0] == '1') {
    for (int i = 0; i < ops.length(); ++ i)
      ops[i] = ops[i] == '0' ? '1' : '0';
    ret = -1 * (stoi(ops, nullptr, 2) + 1);
  } else
    ret = stoi(ops, nullptr, 2);
  return ret;
}

uint32_t bitwise_op(uint32_t a, uint32_t b, char op_type){
  string sa = extend_to_bin(a, 32), sb = extend_to_bin(b, 32);
  for (int i = 0; i < 32; ++ i){
    switch (op_type){
      case '&': {sa[i] = (sa[i]=='1' && sb[i]=='1') ? '1' : '0';break;}
      case '|': {sa[i] = (sa[i]=='1' || sb[i]=='1') ? '1' : '0';break;}
      case '^': {sa[i] = (sa[i]=='1' ^ sb[i]=='1') ? '1' : '0';break;}
      case '!': {sa[i] = !(sa[i]=='1' || sb[i]=='1') ? '1' : '0';break;}
    }
  }
  return bit_2_uint(sa);
}

int line_PC(int line_n){                                        // line_n from 0
  int start = number_to_dec("0x400000");
  return start + line_n * 4;
}

int calc_offset_Addr(int PC, int TC){
  return (TC - (PC+4)) / 4;
}

int calc_offset_line(int C_n, int T_n){
  int ret = (T_n - (C_n+1));
  return ret;
}


string load_byte(uint32_t address){
   int rem = address % 4;
   return extend_to_bin(mem[(address - MEM_START)/4], 32).substr(8*(3-rem),8);
}

void store_byte(uint32_t address, string byte){
   int rem = address % 4;
   string o_word = extend_to_bin(mem[(address - MEM_START)/4], 32) + ' ';
   mem[(address - MEM_START)/4] = bit_2_uint(
          o_word.substr(0,(3-rem)*8) + byte + o_word.substr((4-rem)*8,rem*8)
        );
}

string load_half_word(uint32_t address){
   int rem = (address % 4)/2;
   return extend_to_bin(mem[(address - MEM_START)/4], 32).substr(16*(1-rem),16);
}

void store_half_word(uint32_t address, string half_word){
   int rem = (address % 4)/2;
   string o_word = extend_to_bin(mem[(address - MEM_START)/4], 32) + ' ';
   mem[(address - MEM_START)/4] = bit_2_uint(
          o_word.substr(0,(1-rem)*16) + half_word + o_word.substr(16*(2-rem),rem*16)
        );
}

vector<string> assemble_MIPS(){
    init_args();

    vector<string> MC_INST;
    for (int line_i = 0; line_i < inst_list.size(); ++ line_i){         // deal with each line respectively
        vector<string> line_e = inst_list[line_i];                           // extract line instruction
        string op = line_e[0];
        string MC_INST_i;
        code_type line_c_type = InstMap[op];                            // op's code info

        if (line_c_type.c_type == 'R'){   // R type
            string op_c = line_c_type.op;
            string rs = line_c_type.rs;                 // init each field
            string rt = line_c_type.rt;
            string rd = line_c_type.rd;
            string shamt = line_c_type.shamt;
            string funct = line_c_type.funct;
            string f_order = line_c_type.f_order;   // order of instruction

        for (int i = 0; i < f_order.length(); ++ i){
            if (f_order[i] == 's'){
              rs = RegMap[line_e[1 + i]];
            } else if (f_order[i] == 't'){
              rt = RegMap[line_e[1 + i]];
            } else if (f_order[i] == 'd'){
              rd = RegMap[line_e[1 + i]];
            } else if (f_order[i] == 'h'){
              shamt = extend_to_bin(number_to_dec(line_e[1 + i]), line_c_type.shamt.length());
            } else if (f_order[i] == '_'){
              break;
            }
          }
          MC_INST_i = op_c + rs + rt + rd + shamt + funct;

        } else if (line_c_type.c_type == 'I'){   // I type

            string op_c = line_c_type.op;
            string rs = line_c_type.rs;
            string rt = line_c_type.rt;
            string immediate = line_c_type.immediate;
            string f_order = line_c_type.f_order;

          for (int i = 0; i < f_order.length(); ++ i){
            if (f_order[i] == 's')
              rs = RegMap[line_e[1 + i]];
            else if (f_order[i] == 't')
              rt = RegMap[line_e[1 + i]];
            else if (f_order[i] == 'i')
              immediate = extend_to_bin(number_to_dec(line_e[1 + i]),
                                                line_c_type.immediate.length());
            else if (f_order[i] == 'l'){
              string label_i = line_e[1 + i];
              // cout << "label_i :" << label_i << endl;
              if (label_addr.count(label_i) == 0){
                // int PC = line_PC
                immediate = extend_to_bin(
                  calc_offset_Addr(                             // calc offset
                              line_PC(line_i),                  // calc PC of this line
                              number_to_dec(label_i)),          // calc offset
                  line_c_type.immediate.length());
                // cout << "imm: " << immediate << endl;
              }
              else{
                // cout << "hhhhhh hhhhh" << endl;                              // label_i is label
                immediate = extend_to_bin(
                  calc_offset_line(
                    line_i,
                    label_addr[label_i]                         // line num of label
                    ),
                  line_c_type.immediate.length());
              }
            }
          }

          MC_INST_i = op_c + rs + rt + immediate;

      } else{
          /*                        // J type
             op                address
          | 6bits |            26bits           |
          */

          string op_c = line_c_type.op;
          string address = line_c_type.address;
          string label_i = line_e[1], PC_label;                         // label

          if (label_addr.count(label_i) == 0)
              PC_label = extend_to_bin(number_to_dec(label_i), 32);

          else
              PC_label = extend_to_bin(line_PC(label_addr[label_i]), 32);
          address = PC_label.substr(4, 26);
          MC_INST_i = op_c + address;
      }

      MC_INST.push_back(MC_INST_i);
  }

  return MC_INST;
}

int indicator(uint32_t inst){
    string str = extend_to_bin(inst, 32);
    string op = str.substr(0,6), funct = str.substr(26,6);
    int cnt = 0;
    for (map<string,code_type>::iterator it = InstMap.begin(); it != InstMap.end(); it++, cnt++)
        if (it->second.op == op &&
                          (it->second.c_type != 'R' || it->second.funct == funct))
                return cnt;
    return -1;
}

vector<string> split(string str, string deli){
    const string delims = deli;
    size_t beg, pos = 0;
    vector<string> ret;
    while ((beg = str.find_first_not_of(delims, pos)) != string::npos)
    {
        pos = str.find_first_of(delims, beg + 1);
        ret.push_back(str.substr(beg, pos - beg));
    }
    return ret;
}

void assi_mem(vector<string> MC_INST, string f_data){

  for (int i = 0; i < MC_INST.size(); ++ i)                         // assign memory for .text
      mem[i] = bit_2_uint(MC_INST[i]);

  vector<string> l_data = sep_lines(f_data);

  int p_sta = 1 << 18;

  for (int i = 0; i < l_data.size(); ++ i){
    string line = l_data[i];

    vector<string> str_rm_cmt = split(line, "#"); // remove comment
    if (str_rm_cmt.size() == 0) continue;

    string str_l = str_rm_cmt[0]; // get line content

    // cout << str_l << endl;

    vector<string> str_get_type = split(str_l, ". \t\"");

    // cout << str_get_type.size() << endl;
    if (str_get_type.size() == 0) continue;
    string s_type = str_get_type[1];

    // cout << s_type << endl;

    if (s_type == "ascii" || s_type == "asciiz") {

      vector<string> str_get_str = split(str_l, "\"");

      string str_1 = deal_with_string(str_get_str[1]);

      string str = s_type == "asciiz" ? str_1 + '\0' : str_1;

      vector<string> s_ret;

      for (int j = 0; j < str.length(); ++ j)
        s_ret.push_back(extend_to_bin((int) str[j], 8));

      int max_sup = str.length()%4 == 0 ? 0 : 4 - str.length()%4;
      for (int j = 0; j < max_sup; ++ j)
        s_ret.push_back("00000000");

      uint32_t cPC = p_sta * 4 + MEM_START;

      // cout << "CPC: " << cPC << endl;
      for (int j = 0; j < s_ret.size(); ++ j){
        store_byte(cPC, s_ret[j]);
         cPC ++;
      }

      p_sta += s_ret.size()/4;

    } else if (s_type == "word") {

        vector<string> str_get_str = split(str_l, "\t ,");
        int n_word = str_get_str.size() - 2;
        for (int i = 0; i < n_word; ++ i)
            mem[p_sta++] = bit_2_uint(extend_to_bin(str_get_str[2+i], 32));

    } else if (s_type == "byte") {

        vector<string> str_get_str = split(str_l, "\t ,");
        int n_word = str_get_str.size() - 2;
        int n_sup = n_word % 4 == 0 ? 0 : 4 - n_word%4;         // supplement
        for (int i = 0; i < n_sup; ++ i) str_get_str.push_back("0");

        uint32_t cPC = p_sta * 4 + MEM_START;

        for (int i = 2; i < str_get_str.size(); ++ i){
           store_byte(cPC, extend_to_bin(str_get_str[i], 8));
           cPC ++;
        }

        p_sta += (str_get_str.size()-2)/4;

    } else if (s_type == "half") {
        vector<string> str_get_str = split(str_l, "\t ,");
        int n_word = str_get_str.size() - 2;
        int n_sup = n_word % 2 == 0 ? 0 : 2 - n_word%2;         // supplement
        for (int i = 0; i < n_sup; ++ i) str_get_str.push_back("0");


        uint32_t cPC = p_sta * 4 + MEM_START;

        for (int i = 2; i < str_get_str.size(); ++ i){
           store_half_word(cPC, extend_to_bin(str_get_str[i], 16));
           cPC += 2;
        }

        p_sta += (str_get_str.size()-2)/2;

    }

  }
    heapHead = p_sta * 4 + MEM_START;
//    cout << p_sta << endl;
    // initialize registers
    reg[0] = 0;
    reg[28] = 5242880;
    reg[29] = 10485760;
    reg[30] = 10485760;
}

int addr_2_line_n(uint32_t addr){
    return (int) (addr - 4194304) >> 2;
}

vector<string> decom_inst(uint32_t inst, string inst_name){
  string str = extend_to_bin(inst, 32);
  char c_type = InstMap[inst_name].c_type;
  vector<string> ret;

  switch(c_type){
      case 'R':
         ret.push_back(str.substr(6,5));  // rs
         ret.push_back(str.substr(11,5));  // rt
         ret.push_back(str.substr(16,5));  // rd
         ret.push_back(str.substr(21,5));  // shamt
         break;
      case 'I':
         ret.push_back(str.substr(6,5));  // rs
         ret.push_back(str.substr(11,5));  // rt
         ret.push_back(str.substr(16,16));  // immediate
         break;
      case 'J':
         ret.push_back(str.substr(6,26));  // address
         break;
      default:
         cout << "No such type" << endl;
         break;
  }
  return ret;
}

bool isOverflow(uint32_t a, uint32_t b, uint32_t c){
  char ca = extend_to_bin(a, 32)[0];
  char cb = extend_to_bin(b, 32)[0];
  char cc = extend_to_bin(c, 32)[0];
  return (cb == cc && cb != ca);
}

uint32_t two_cmp(uint32_t a){
    string str = extend_to_bin(a, 32);
    for (int i = 0; i < 32; ++ i)
        str[i] = (str[i] == '1') ? '0' : '1';
    return bit_2_uint(str) + 1;
}

string bin_sign_extend(string bin, int len){
    return string(len-bin.length(), bin[0]) + bin;
}

string bin_unsign_extend(string bin, int len){
    return string(len-bin.length(), '0') + bin;
}

void _trap() { cout << "Trap!" << endl; }

uint32_t count_ldg(uint32_t x, char c){
  string str = extend_to_bin(x, 32);
  uint32_t ret = 0;
  for (int i = 0; str[i] == c && i < 32; ++ i) ++ ret;
  return ret;
}

void fileIO(char mode){
  switch(mode) {

    case 'o': {
        // open
        int flg = bit_2_int_signed(extend_to_bin(reg[5],32));
        int mode = bit_2_int_signed(extend_to_bin(reg[6],32));
        uint32_t addr = (reg[4] - MEM_START) >> 2;
//        cout << "file name: " << (char*)(mem+addr) << endl;
        int fd = open((char*)(mem+addr), flg, mode);
//        cout << "open fd: " << fd << endl;
        reg[2] = (uint32_t) fd;
//      cout << "fd: " << reg[4] << endl;
        break;
    }
    case 'r': {
        int fd = bit_2_int_signed(extend_to_bin(reg[4],32));
        int addr = (reg[5] - MEM_START) >> 2;
        int len = bit_2_int_signed(extend_to_bin(reg[6],32));
        int r_len = read(fd, (char*)(mem+addr), len);
        reg[2] = bit_2_uint(extend_to_bin(r_len,32));
        break;
    }
    case 'w': {
        int fd = (int) reg[4];
//        cout << "t0: " << reg[8] << " fd: " << fd << endl;
        int addr = (reg[5] - MEM_START) >> 2;
        int len = bit_2_int_signed(extend_to_bin(reg[6],32));
//        cout << (char*)(mem+addr) << " len:" << len << endl;
        int r_len = write(fd, (char*)(mem+addr), len);
//        cout << "write: " << fd << " " << r_len << endl;
        reg[2] = bit_2_uint(extend_to_bin(r_len,32));
        break;
    }
    case 'c': {
        close( bit_2_int_signed(extend_to_bin(reg[4],32)) );
        break;
    }
  }
}


int min(int a, int b){
    return a < b ? a : b;
}

void simulate(int n_inst, string s_input, string s_output){

    inFile.open(s_input);
//    cout << "output file name: " << s_output << endl;
    outFile.open(s_output);
//    ofp.open;
//    outFile << "hello";
//    outFile << "world";
  // freopen("apb.in","r",stdin);
//    ifstream input2(stream_name);
    uint32_t PC = MEM_START;
    while ((PC - MEM_START)/4 < n_inst){
        uint32_t inst_i = mem[(PC - MEM_START)/4];    // extract instruction

        int indc = indicator(inst_i);
        map<string,code_type>::iterator iter_1 = InstMap.begin();
        for (int i = 0; i < indc; ++ i) iter_1++;
        string inst_name = iter_1->first;
        vector<string> fields = decom_inst(inst_i, inst_name);// fileds

        bool flg_T = false;

//         cout << inst_name << endl;
        switch(indc){
            case 0:{       //add
               int rs = bit_2_int(fields[0]);
               int rt = bit_2_int(fields[1]);
               int rd = bit_2_int(fields[2]);
               reg[rd] = reg[rs] + reg[rt];
               if (isOverflow(reg[rd], reg[rs], reg[rt])){
                  cout << "Error: Overflow detected!" << endl; flg_T = true;
               }
               break;
            }
            case 1:{         //addi
               int rs = bit_2_int(fields[0]);
               int rt = bit_2_int(fields[1]);
               uint32_t immediate = bit_2_uint(bin_sign_extend(fields[2],32));
               reg[rt] = reg[rs] + immediate;
               if (isOverflow(reg[rt], reg[rs], immediate)){
                  cout << "Error: Overflow detected!" << endl; flg_T = true;
               }
               break;
            }
            case 2:{         //addiu
               int rs = bit_2_int(fields[0]);
               int rt = bit_2_int(fields[1]);
               uint32_t imm = bit_2_uint(fields[2]);
               reg[rt] = reg[rs] + imm;
               break;
            }
            case 3:{         //addu
               int rs = bit_2_int(fields[0]);
               int rt = bit_2_int(fields[1]);
               int rd = bit_2_int(fields[2]);
               reg[rd] = reg[rs] + reg[rt];
               break;
            }
            case 4:{         //and
               int rs = bit_2_int(fields[0]);
               int rt = bit_2_int(fields[1]);
               int rd = bit_2_int(fields[2]);
               reg[rd] = bitwise_op(reg[rs], reg[rt], '&');
               break;
            }
            case 5:{         //andi
               int rs = bit_2_int(fields[0]);
               int rt = bit_2_int(fields[1]);
               uint32_t imm = bit_2_uint(bin_sign_extend(fields[2],32));
               reg[rt] = bitwise_op(reg[rs], imm, '&');
               break;
            }
            case 6:{         //beq
               int rs = bit_2_int(fields[0]);
               int rt = bit_2_int(fields[1]);
               int offset = bit_2_int_signed(fields[2]);
               if (reg[rs] == reg[rt]) {
                  PC += offset * 4;   // -4 for +4 later
                }
               break;
            }
            case 7:{         //bgez
               int rs = bit_2_int(fields[0]);
               int offset = bit_2_int_signed(fields[2]);
               int v_reg = bit_2_int_signed(extend_to_bin(reg[rs], 32));
               if (v_reg >= 0) PC += offset * 4;   // -4 for +4 later
               break;
            }
            case 8:{         //bgezal
               int rs = bit_2_int(fields[0]);
               int offset = bit_2_int_signed(fields[2]);
               int v_reg = bit_2_int_signed(extend_to_bin(reg[rs], 32));
               if (v_reg >= 0) {
                  reg[31] = PC + 4;
                  PC += offset * 4;
               }
               break;
            }
            case 9:{         //bgtz
               int rs = bit_2_int(fields[0]);
               int offset = bit_2_int_signed(fields[2]);
               int v_reg = bit_2_int_signed(extend_to_bin(reg[rs], 32));
               if (v_reg > 0) PC += offset * 4;   // -4 for +4 later
               break;
            }
            case 10:{         //blez
               int rs = bit_2_int(fields[0]);
               int offset = bit_2_int_signed(fields[2]);
               int v_reg = bit_2_int_signed(extend_to_bin(reg[rs], 32));
               if (v_reg <= 0) {
                  PC += offset * 4;   // -4 for +4 later
                }
               else
               break;
            }
            case 11:{         //bltz
               int rs = bit_2_int(fields[0]);
               int offset = bit_2_int_signed(fields[2]);
               int v_reg = bit_2_int_signed(extend_to_bin(reg[rs], 32));
               if (v_reg < 0) PC += offset * 4;   // -4 for +4 later
               break;
            }
            case 12:{         //bltzal
               int rs = bit_2_int(fields[0]);
               int offset = bit_2_int_signed(fields[2]);
               int v_reg = bit_2_int_signed(extend_to_bin(reg[rs], 32));
               if (v_reg < 0) {
                  reg[31] = PC + 4;
                  PC += offset * 4;
               }
               break;
            }
            case 13:{         //bne
               int rs = bit_2_int(fields[0]);
               int rt = bit_2_int(fields[1]);
               int offset = bit_2_int_signed(fields[2]);
               if (reg[rs] != reg[rt]) PC += offset * 4;   // -4 for +4 later
               break;
            }
            case 14:{         //clo
               int rs = bit_2_int(fields[0]);
               int rd = bit_2_int(fields[2]);
               reg[rd] = count_ldg(reg[rs], '1');
               break;
            }
            case 15:{         //clz
               int rs = bit_2_int(fields[0]);
               int rd = bit_2_int(fields[2]);
               reg[rd] = count_ldg(reg[rs], '0');
               break;
            }
            case 16:{         //div
               int rs = bit_2_int(fields[0]);
               int rt = bit_2_int(fields[1]);
               int v_rs = bit_2_int_signed(extend_to_bin(reg[rs], 32));
               int v_rt = bit_2_int_signed(extend_to_bin(reg[rt], 32));
               reg[LO] = bit_2_uint(extend_to_bin(v_rs/v_rt, 32));
               reg[HI] = bit_2_uint(extend_to_bin(v_rs%v_rt, 32));
               break;
            }
            case 17:{         //divu
               int rs = bit_2_int(fields[0]);
               int rt = bit_2_int(fields[1]);
               uint32_t v_rs = bit_2_uint(extend_to_bin(reg[rs], 32));
               uint32_t v_rt = bit_2_uint(extend_to_bin(reg[rt], 32));
               reg[LO] = bit_2_uint(extend_to_bin(v_rs/v_rt, 32));
               reg[HI] = bit_2_uint(extend_to_bin(v_rs%v_rt, 32));
               break;
            }
            case 18:{         //j
               uint32_t offset = bit_2_uint(fields[0]);
               uint32_t nPC = (uint32_t) (4026531840 & PC) | (offset << 2);
               PC = nPC - 4;
               break;
            }
            case 19:{         //jal
               uint32_t offset = bit_2_uint(fields[0]);
               uint32_t nPC = (uint32_t) (4026531840 & PC) | (offset << 2);
               reg[31] = PC + 4;
               PC = nPC - 4;
               break;
            }
            case 20:{         //jalr
               int rs = bit_2_int(fields[0]);
               int rd = bit_2_int(fields[2]);
               uint32_t address = reg[rs];
               reg[rd] = PC + 4;
               PC = address - 4;
               break;
            }
            case 21:{         //jr
               int rs = bit_2_int(fields[0]);
               uint32_t address = reg[rs];
               PC = address - 4;
               break;
            }
            case 22:{         //lb
               int base = bit_2_int(fields[0]);
               int rt = bit_2_int(fields[1]);
               int offset = bit_2_int_signed(fields[2]);
               uint32_t address = reg[base] + offset;
               reg[rt] = bit_2_uint(bin_sign_extend(load_byte(address), 32));
               break;
            }
            case 23:{         //lbu
               int base = bit_2_int(fields[0]);
               int rt = bit_2_int(fields[1]);
               int offset = bit_2_int_signed(fields[2]);
               int address = reg[base] + offset;
               reg[rt] = bit_2_uint(bin_unsign_extend(load_byte(address), 32));
               break;
            }
            case 24:{         //lh
               int base = bit_2_int(fields[0]);
               int rt = bit_2_int(fields[1]);
               int offset = bit_2_int_signed(fields[2]);
               uint32_t address = reg[base] + offset;
               reg[rt] = bit_2_uint(bin_sign_extend(load_half_word(address), 32));
               break;
            }
            case 25:{         //lhu
               int base = bit_2_int(fields[0]);
               int rt = bit_2_int(fields[1]);
               int offset = bit_2_int_signed(fields[2]);
               uint32_t address = reg[base] + offset;
               reg[rt] = bit_2_uint(bin_unsign_extend(load_half_word(address), 32));
               break;
            }
            case 26:{         //ll
               int base = bit_2_int(fields[0]);
               int rt = bit_2_int(fields[1]);
               int offset = bit_2_int_signed(fields[2]);
               uint32_t address = reg[base] + offset;
               int p_line = (address - 4194304) / 4;
               reg[rt] = mem[p_line];
               break;
            }
            case 27:{         //lui
               int rt = bit_2_int(fields[1]);
               int immediate = bit_2_int_signed(fields[2]);
               reg[rt] = bit_2_uint(extend_to_bin(immediate<<16, 32));
               break;
            }
            case 28:{         //lw
               int base = bit_2_int(fields[0]);
               int rt = bit_2_int(fields[1]);
               int offset = bit_2_int_signed(fields[2]);
               uint32_t address = reg[base] + offset;
               int p_line = (address - 4194304) / 4;
               reg[rt] = mem[p_line];
               break;
            }
            case 29:{         //lwl
               int base = bit_2_int(fields[0]);
               int rt = bit_2_int(fields[1]);
               int offset = bit_2_int(bin_sign_extend(fields[2],32));
               uint32_t address = reg[base] + offset;
               int rem = address % 4;
               string extract_w = extend_to_bin(mem[(address - MEM_START)/4], 32).substr(8*(3-rem),(rem+1)*8);
               string s_rt = extend_to_bin(reg[rt], 32) + ' ';
               reg[rt] = bit_2_uint(
                        extract_w + s_rt.substr((rem+1)*8, (3-rem)*8)
                    );
               break;
            }
            case 30:{         //lwr
               int base = bit_2_int(fields[0]);
               int rt = bit_2_int(fields[1]);
               int offset = bit_2_int(bin_sign_extend(fields[2],32));
               uint32_t address = reg[base] + offset;
               int rem = address % 4;
               string extract_w = extend_to_bin(mem[(address - MEM_START)/4], 32).substr(0,8*(4-rem));
               string s_rt = extend_to_bin(reg[rt], 32);
               reg[rt] = bit_2_uint(
                         s_rt.substr(0,rem*8) + extract_w
                    );
               break;
            }
            case 31:{         //madd
               int rs = bit_2_int(fields[0]);
               int rt = bit_2_int(fields[1]);
               int v_rs = bit_2_int_signed(extend_to_bin(reg[rs], 32));
               int v_rt = bit_2_int_signed(extend_to_bin(reg[rt], 32));
               int64_t v_ori = ((int64_t) reg[HI] << 32) + reg[LO];
               int64_t v_mult = (int64_t) v_rs * v_rt + v_ori;
               reg[LO] = bit_2_uint(bitset<64>(v_mult).to_string().substr(32, 32));
               reg[HI] = bit_2_uint(bitset<64>(v_mult).to_string().substr(0, 32));
               break;
            }
            case 32:{         //maddu
               int rs = bit_2_int(fields[0]);
               int rt = bit_2_int(fields[1]);
               uint64_t v_ori = ((uint64_t) reg[HI] << 32) + reg[LO];
               uint64_t v_mult =(uint64_t) reg[rs] * reg[rt] + v_ori;
               reg[LO] = bit_2_uint(bitset<64>(v_mult).to_string().substr(32, 32));
               reg[HI] = bit_2_uint(bitset<64>(v_mult).to_string().substr(0, 32));
               break;
            }
            case 33:{         //mfhi
               int rd = bit_2_int(fields[2]);
               reg[rd] = reg[HI];
               break;
            }
            case 34:{         //mflo
               int rd = bit_2_int(fields[2]);
               reg[rd] = reg[LO];
               break;
            }
            case 35:{         //msub
               int rs = bit_2_int(fields[0]);
               int rt = bit_2_int(fields[1]);
               int v_rs = bit_2_int_signed(extend_to_bin(reg[rs], 32));
               int v_rt = bit_2_int_signed(extend_to_bin(reg[rt], 32));
               int64_t v_ori = ((int64_t) reg[HI] << 32) + reg[LO];
               int64_t v_mult = (int64_t) v_ori - (int64_t) v_rs * v_rt;
               reg[LO] = bit_2_uint(bitset<64>(v_mult).to_string().substr(32, 32));
               reg[HI] = bit_2_uint(bitset<64>(v_mult).to_string().substr(0, 32));
               break;
            }
            case 36:{         //msubu
               int rs = bit_2_int(fields[0]);
               int rt = bit_2_int(fields[1]);
               uint64_t v_ori = ((uint64_t) reg[HI] << 32) + reg[LO];
               uint64_t v_mult = (uint64_t) v_ori - (uint64_t) reg[rs] * reg[rt];
               reg[LO] = bit_2_uint(bitset<64>(v_mult).to_string().substr(32, 32));
               reg[HI] = bit_2_uint(bitset<64>(v_mult).to_string().substr(0, 32));
               break;
            }
            case 37:{         //mthi
               int rs = bit_2_int(fields[0]);
               reg[HI] = reg[rs];
               break;
            }
            case 38:{         //mtlo
               int rs = bit_2_int(fields[0]);
               reg[LO] = reg[rs];
               break;
            }
            case 39:{         //mul
               int rs = bit_2_int(fields[0]);
               int rt = bit_2_int(fields[1]);
               int rd = bit_2_int(fields[2]);
               int v_rs = bit_2_int_signed(extend_to_bin(reg[rs], 32));
               int v_rt = bit_2_int_signed(extend_to_bin(reg[rt], 32));
               int64_t v_mult = (int64_t) v_rs * v_rt;
               reg[rd] = bit_2_uint(bitset<64>(v_mult).to_string().substr(32, 32));
               break;
            }
            case 40:{         //mult
               int rs = bit_2_int(fields[0]);
               int rt = bit_2_int(fields[1]);
               int v_rs = bit_2_int_signed(extend_to_bin(reg[rs], 32));
               int v_rt = bit_2_int_signed(extend_to_bin(reg[rt], 32));
               int64_t v_mult = (int64_t) v_rs * v_rt;
               reg[LO] = bit_2_uint(bitset<64>(v_mult).to_string().substr(32, 32));
               reg[HI] = bit_2_uint(bitset<64>(v_mult).to_string().substr(0, 32));
               break;
            }
            case 41:{         //multu
               int rs = bit_2_int(fields[0]);
               int rt = bit_2_int(fields[1]);
               uint64_t v_mult = (uint64_t) reg[rs] * reg[rt];
               reg[LO] = bit_2_uint(bitset<64>(v_mult).to_string().substr(32, 32));
               reg[HI] = bit_2_uint(bitset<64>(v_mult).to_string().substr(0, 32));
               break;
            }
            case 42:{         //nor
               int rs = bit_2_int(fields[0]);
               int rt = bit_2_int(fields[1]);
               int rd = bit_2_int(fields[2]);
               reg[rd] = bitwise_op(reg[rs], reg[rt], '!');
               break;
            }
            case 43:{         //or
               int rs = bit_2_int(fields[0]);
               int rt = bit_2_int(fields[1]);
               int rd = bit_2_int(fields[2]);
               reg[rd] = bitwise_op(reg[rs], reg[rt], '|');
               break;
            }
            case 44:{         //ori
               int rs = bit_2_int(fields[0]);
               int rt = bit_2_int(fields[1]);
               uint32_t imm = bit_2_uint(bin_sign_extend(fields[2],32));
               reg[rt] = bitwise_op(reg[rs], imm, '|');
               break;
            }
            case 45:{         //sb
               int base = bit_2_int(fields[0]);
               int rt = bit_2_int(fields[1]);
               int offset = bit_2_int_signed(fields[2]);
               uint32_t address = reg[base] + offset;
               store_byte(address, extend_to_bin(reg[rt],8));
               break;
            }
            case 46:{         //sc
               int base = bit_2_int(fields[0]);
               int rt = bit_2_int(fields[1]);
               int offset = bit_2_int_signed(fields[2]);
               uint32_t address = reg[base] + offset;
               int p_line = (address - MEM_START) / 4;
               mem[p_line] = reg[rt];
               break;
            }
            case 47:{         //sh
               int base = bit_2_int(fields[0]);
               int rt = bit_2_int(fields[1]);
               int offset = bit_2_int_signed(fields[2]);
               uint32_t address = reg[base] + offset;
               store_half_word(address, extend_to_bin(reg[rt],16));
               break;
            }
            case 48:{         //sll
               int rt = bit_2_int(fields[1]);
               int rd = bit_2_int(fields[2]);
               int sa = bit_2_int(fields[3]);
               reg[rd] = reg[rt] << sa;
               break;
            }
            case 49:{         //sllv
               int rs = bit_2_int(fields[0]);
               int rt = bit_2_int(fields[1]);
               int rd = bit_2_int(fields[2]);
               reg[rd] = reg[rt] << reg[rs];
               break;
            }
            case 50:{         //slt
               int rs = bit_2_int(fields[0]);
               int rt = bit_2_int(fields[1]);
               int rd = bit_2_int(fields[2]);
               int v_rs = bit_2_int_signed(extend_to_bin(reg[rs], 32));
               int v_rt = bit_2_int_signed(extend_to_bin(reg[rt], 32));
               reg[rd] = v_rs < v_rt ? 1 : 0;
               break;
            }
            case 51:{         //slti
               int rs = bit_2_int(fields[0]);
               int rt = bit_2_int(fields[1]);
               int immediate = bit_2_int_signed((bin_sign_extend(fields[2],32)));
               int v_rs = bit_2_int_signed(extend_to_bin(reg[rs], 32));
               reg[rt] = v_rs < immediate ? 1 : 0;
               break;
            }
            case 52:{         //sltiu
               int rs = bit_2_int(fields[0]);
               int rt = bit_2_int(fields[1]);
               uint32_t immediate = bit_2_uint((bin_unsign_extend(fields[2],32)));
               uint32_t v_rs = bit_2_uint(extend_to_bin(reg[rs], 32));
               reg[rt] = v_rs < immediate ? 1 : 0;
               break;
            }
            case 53:{         //sltu
               int rs = bit_2_int(fields[0]);
               int rt = bit_2_int(fields[1]);
               int rd = bit_2_int(fields[2]);
               uint32_t v_rs = bit_2_uint(extend_to_bin(reg[rs], 32));
               uint32_t v_rt = bit_2_uint(extend_to_bin(reg[rt], 32));
               reg[rd] = v_rs < v_rt ? 1 : 0;
               break;
            }
            case 54:{         //sra
               int rt = bit_2_int(fields[1]);
               int rd = bit_2_int(fields[2]);
               int sa = bit_2_int(fields[3]);
               string o_word = extend_to_bin(reg[rt], 32);
               string n_word = string(sa, o_word[0]) + o_word;
               reg[rd] = bit_2_uint(n_word.substr(0,32));
               break;
            }
            case 55:{         //srav
               int rs = bit_2_int(fields[0]);
               int rt = bit_2_int(fields[1]);
               int rd = bit_2_int(fields[2]);
               string o_word = extend_to_bin(reg[rt], 32);
               string n_word = string(reg[rd], o_word[0]) + o_word;
               reg[rd] = bit_2_uint(n_word.substr(0,32));
               break;
            }
            case 56:{         //srl
               int rt = bit_2_int(fields[1]);
               int rd = bit_2_int(fields[2]);
               int sa = bit_2_int(fields[3]);
               string o_word = extend_to_bin(reg[rt], 32);
               string n_word = string(sa, '0') + o_word;
               reg[rd] = bit_2_uint(n_word.substr(0,32));
               break;
            }
            case 57:{         //srlv
               int rs = bit_2_int(fields[0]);
               int rt = bit_2_int(fields[1]);
               int rd = bit_2_int(fields[2]);
               string o_word = extend_to_bin(reg[rt], 32);
               string n_word = string(reg[rd], '0') + o_word;
               reg[rd] = bit_2_uint(n_word.substr(0,32));
               break;
            }
            case 58:{         //sub
               int rs = bit_2_int(fields[0]);
               int rt = bit_2_int(fields[1]);
               int rd = bit_2_int(fields[2]);
               unsigned v_neg_rt = two_cmp(reg[rt]);
               reg[rd] = reg[rs] + v_neg_rt;
               if (isOverflow(reg[rd], reg[rs], v_neg_rt)){
                  cout << "Error: Overflow detected!" << endl; flg_T = true;
               }
               break;
            }
            case 59:{         //subu
               int rs = bit_2_int(fields[0]);
               int rt = bit_2_int(fields[1]);
               int rd = bit_2_int(fields[2]);
               unsigned v_neg_rt = two_cmp(reg[rt]);
               reg[rd] = reg[rs] + v_neg_rt;
               break;
            }
            case 60:{         //sw
               int base = bit_2_int(fields[0]);
               int rt = bit_2_int(fields[1]);
               int offset = bit_2_int_signed(fields[2]);
               uint32_t address = reg[base] + offset;
               int p_line = (address - MEM_START) / 4;
               mem[p_line] = reg[rt];
               break;
            }
            case 61:{         //swl
               int base = bit_2_int(fields[0]);
               int rt = bit_2_int(fields[1]);
               int offset = bit_2_int(bin_sign_extend(fields[2],32));
               uint32_t address = reg[base] + offset;
               int rem = address % 4;
               string extract_rt = extend_to_bin(reg[rt], 32).substr(0,(rem+1)*8);
               string s_wd = extend_to_bin(mem[(address - MEM_START)/4], 32).substr(0, (30-rem)*8);
               mem[(address - MEM_START)/4] = bit_2_uint(
                        s_wd + extract_rt
                    );
               break;
            }
            case 62:{         //swr
               int base = bit_2_int(fields[0]);
               int rt = bit_2_int(fields[1]);
               int offset = bit_2_int(bin_sign_extend(fields[2],32));
               uint32_t address = reg[base] + offset;
               int rem = address % 4;
               string extract_rt = extend_to_bin(reg[rt], 32).substr(rem*8, (4-rem)*8);
               string s_wd = (extend_to_bin(mem[(address - MEM_START)/4], 32) + ' ').substr((4-rem)*8, rem*8);
               mem[(address - MEM_START)/4] = bit_2_uint(
                        extract_rt + s_wd
                    );
               break;
            }
            case 63:{         //syscall

               int syscall_code = (int) reg[2];      // $v0
               switch(syscall_code){
                    case 1: {   // print int
//                        string s = to_string((int) reg[4]);
//                        cout << "print int: " << (int) reg[4] << endl;
                        outFile << (int) reg[4];
                        break;
                    }
                    case 4: {   // print string
                        uint32_t address = (reg[4] - MEM_START) >> 2;
//                        cout << "address:" << reg[4] << endl;
//                        cout << (char *) (mem+address);
                        outFile << (char *) (mem+address);
//                        fputs((char *) (mem+address), ofp);
                        break;
                    }
                    case 5: {   // read int
                        int in;
                        string line;
                        getline(inFile, line);
                        in = number_to_dec(line);
//                        geline()
//                        cout << "read int : " << in << endl;
//                        reg[2] = bit_2_uint(extend_to_bin(in,32));
                        reg[2] = (uint32_t) in;
                        break;
                    }
                    case 8: {     // read string
//                      int addr = ((int) reg[4] - MEM_START) >> 2;
                      int len = (int) reg[5];
                      string s_in;
                      getline(inFile, s_in);
                      string s_line = deal_with_string(s_in);
                      for (int i = 0 ; i < min(len,s_line.length()); ++ i){
                          store_byte(reg[4], extend_to_bin((uint32_t)s_line[i],8));
                      }
//                      read(1, &mem[addr], len);

                      break;
                  }
                    case 9: {     // sbrk
                      reg[2] = heapHead;
                      heapHead += bit_2_int_signed(extend_to_bin(reg[4],32));
                      break;
                  }
                    case 10: { exit(0); }
                    case 11: {    // print char
//                        char c = (char) bit_2_int(extend_to_bin(reg[4], 8));
//                      cout << c;
                        outFile << (char) reg[4];
                    break;
                  }
                    case 12: {    // read char
                        char c;
                        string line;
                        getline(inFile, line);
                        c = line[0];
                        reg[2] = (uint32_t) c;
//                      reg[2] = bit_2_uint(extend_to_bin((int)c, 32));
                    break;
                  }
                  case 13: {fileIO('o');break;}
                  case 14: {fileIO('r');break;}
                  case 15: {fileIO('w'); break;}
                  case 16: {fileIO('c'); break;}
                  case 17: { exit(bit_2_int_signed(extend_to_bin(reg[4],32))); }
               }
               break;
            }
            case 64:{         //teq
               int rs = bit_2_int(fields[0]);
               int rt = bit_2_int(fields[1]);
               if (reg[rs] == reg[rt]) { cout << "Trap: Equal" << endl; exit(0);}
               break;
            }
            case 65:{         //teqi
               int rs = bit_2_int(fields[0]);
               int rt = bit_2_int(fields[1]);
               int immediate = bit_2_int_signed(bin_sign_extend(fields[2],32));
               int v_rs = bit_2_int_signed(extend_to_bin(reg[rs], 32));
               if (v_rs == immediate) { cout << "Trap: Equal" << endl; exit(0);}
               break;
            }
            case 66:{         //tge
               int rs = bit_2_int(fields[0]);
               int rt = bit_2_int(fields[1]);
               int v_rs = bit_2_int_signed(extend_to_bin(reg[rs], 32));
               int v_rt = bit_2_int_signed(extend_to_bin(reg[rt], 32));
               // int rd = bit_2_int(fields[2]);
               if (v_rs >= v_rt) { _trap(); flg_T=true; }
               break;
            }
            case 67:{         //tgei
               int rs = bit_2_int(fields[0]);
               int rt = bit_2_int(fields[1]);
               int immediate = bit_2_int_signed(bin_sign_extend(fields[2],32));
               int v_rs = bit_2_int_signed(extend_to_bin(reg[rs], 32));
               if (v_rs >= immediate) { _trap(); flg_T=true; }
               break;
            }
            case 68:{         //tgeiu
               int rs = bit_2_int(fields[0]);
               int rt = bit_2_int(fields[1]);
               uint32_t immediate = bit_2_uint(bin_unsign_extend(fields[2],32));
               uint32_t v_rs = bit_2_uint(extend_to_bin(reg[rs], 32));
               if (v_rs >= immediate) { _trap(); flg_T=true; }
               break;
            }
            case 69:{         //tgeu
               int rs = bit_2_int(fields[0]);
               int rt = bit_2_int(fields[1]);
               uint32_t v_rs = bit_2_uint(extend_to_bin(reg[rs], 32));
               uint32_t v_rt = bit_2_uint(extend_to_bin(reg[rt], 32));
               // int rd = bit_2_int(fields[2]);
               if (v_rs >= v_rt) { _trap(); flg_T=true; }
               break;
            }
            case 70:{         //tlt
               int rs = bit_2_int(fields[0]);
               int rt = bit_2_int(fields[1]);
               int v_rs = bit_2_int_signed(extend_to_bin(reg[rs], 32));
               int v_rt = bit_2_int_signed(extend_to_bin(reg[rt], 32));
               // int rd = bit_2_int(fields[2]);
               if (v_rs < v_rt) { _trap(); flg_T=true; }
               break;
            }
            case 71:{         //tlti
               int rs = bit_2_int(fields[0]);
               int rt = bit_2_int(fields[1]);
               int immediate = bit_2_int_signed(bin_sign_extend(fields[2],32));
               int v_rs = bit_2_int_signed(extend_to_bin(reg[rs], 32));
               if (v_rs < immediate) { _trap(); flg_T=true; }
               break;
            }
            case 72:{         //tltiu
               int rs = bit_2_int(fields[0]);
               int rt = bit_2_int(fields[1]);
               uint32_t immediate = bit_2_uint(bin_unsign_extend(fields[2],32));
               uint32_t v_rs = bit_2_uint(extend_to_bin(reg[rs], 32));
               if (v_rs < immediate) { _trap(); flg_T=true; }
               break;
            }
            case 73:{         //tltu
               int rs = bit_2_int(fields[0]);
               int rt = bit_2_int(fields[1]);
               uint32_t v_rs = bit_2_uint(extend_to_bin(reg[rs], 32));
               uint32_t v_rt = bit_2_uint(extend_to_bin(reg[rt], 32));
               // int rd = bit_2_int(fields[2]);
               if (v_rs < v_rt) { _trap(); flg_T=true; }
               break;
            }
            case 74:{         //tne
               int rs = bit_2_int(fields[0]);
               int rt = bit_2_int(fields[1]);
               // int rd = bit_2_int(fields[2]);
               if (reg[rs] != reg[rt]) { _trap(); flg_T=true; }
               break;
            }
            case 75:{         //tnei
               int rs = bit_2_int(fields[0]);
               int rt = bit_2_int(fields[1]);
               int immediate = bit_2_int_signed(bin_sign_extend(fields[2],32));
               int v_rs = bit_2_int_signed(extend_to_bin(reg[rs], 32));
               if (v_rs != immediate) { _trap(); flg_T=true; }
               break;
            }
            case 76:{         //xor
               int rs = bit_2_int(fields[0]);
               int rt = bit_2_int(fields[1]);
               int rd = bit_2_int(fields[2]);
               reg[rd] = bitwise_op(reg[rs], reg[rt], '^');
               break;
            }
            case 77:{         //xori
               int rs = bit_2_int(fields[0]);
               int rt = bit_2_int(fields[1]);
               uint32_t imm = bit_2_uint(bin_sign_extend(fields[2],32));
               reg[rt] = bitwise_op(reg[rs], imm, '^');
               break;
            }
            default:{
               cout << "Error: No such instruction -> " << extend_to_bin(inst_i, 32) << endl;
               break;
            }
        }

        if (flg_T) break;
        PC += (uint32_t) 4;
    }
}


int main(int argc, char** argv){
    if (argc < 4){
        printf("wrong number of arguments");
    }

    ifstream ifs(argv[1]);
    string f_str((istreambuf_iterator<char>(ifs)),
               (istreambuf_iterator<char>() ));

    map<string, string> f_sec = extract_text_seg(f_str);

    sign_label_addr(f_sec["text"]);

    vector<string> MC_INST = assemble_MIPS();

//  cout << "success assemble" << endl;
    assi_mem(MC_INST, f_sec["data"]);
    simulate(MC_INST.size(), argv[2], argv[3]);


    return 0;
}









