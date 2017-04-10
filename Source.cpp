#include <stdio.h>
#include "str.h"
#include <conio.h>
#include <fstream>

using std::ios;
using std::cin;
using std::cout;
using std::fstream;


fstream fp_index;
fstream fp_db;


 	
struct f_rec {	// Financial record structure
   int    num;	// Transaction number
   float  amt;	// Amount of the transaction
   char   type;	// Transaction type (D=debit, C=credit)
   int    acct;	// Account used for transaction
   int    hour;	// Hour of transaction (0-23)
   int    min;	// Minute of transaction (0-59)
   int    day;	// Day of transaction (0-364)
   int    year;	// Year of transaction (0000-9999)
}db_record;

	
struct ch_node {	// Chained hash table node
  int   k;	        // Key of record node represents
  long  rec;	    // Offset of corresponding record in database file
  long  next;	    // Offset of next node in this chain (-1 for none)
}index_node,temp_index_node;   

int write_i=0;
long db_pointer=0;
long temp_ptr_update;
int hash_size=1001;
int hash_value;
int first_hash_off=0;
int traverse_ptr;
long ret_pointer;
string token[20];


void create_file_index(string file_name)
{
	fp_db.open(file_name, ios::in | ios::out | ios::app | ios::binary);
	fp_db.close();
	fp_db.open(file_name, ios::in | ios::out | ios::binary);
}
void create_file_db(string file_name)
{
	fp_index.open(file_name, ios::in | ios::out | ios::app | ios::binary);
	fp_index.close();
	fp_index.open(file_name, ios::in | ios::out | ios::binary);
}

long search(int key_pass, string type)
{

	fp_index.seekp(hash_value*(sizeof (ch_node)));
	
	while(1)
	{
		fp_index.read((char*) &temp_index_node, sizeof (ch_node));
		
		if(temp_index_node.k==key_pass && type=="add")
		{
			cout<<"Record "<<key_pass<<" is a duplicate.\n";
			return -1;
			break;
		}
		else if(temp_index_node.k==key_pass && type=="find")
		{
			return temp_index_node.rec;
		}
		else if(temp_index_node.k==key_pass && type=="delete")
		{
			temp_index_node.k=-1;
			long delete_index_ptr=fp_index.tellp();
			return delete_index_ptr-sizeof(ch_node);
		}
		else if(temp_index_node.next==-1)
		{
			if(type=="add")
			{
			traverse_ptr=fp_index.tellp();
			return traverse_ptr-(sizeof (ch_node)); 
			break;
			}
			if(type=="find")
			{
			return -1;
			break;
			}
			if(type=="delete")
			{
			return -1;
			break;
			}
		}
		else
		{
		fp_index.seekp(temp_index_node.next,ios::beg);
		}
	
	
	}
	
}

void add_record(int key)
{
	index_node.k=key;
	index_node.rec=db_pointer;
	index_node.next=-1;
	hash_value=key%hash_size;
	ret_pointer=search(key,"add");
	if(ret_pointer!=-1)
	{
	if(temp_index_node.k==-1)
	{
		fp_index.seekp(ret_pointer,ios::beg);
		fp_index.write((char*) &index_node, sizeof (ch_node));
	}
	else
	{
	fp_index.seekp(0,ios::end);
	temp_ptr_update=fp_index.tellp();
	fp_index.write((char*) &index_node, sizeof (ch_node));
	fp_index.seekp(ret_pointer,ios::beg);
	temp_index_node.next=temp_ptr_update;
	fp_index.write((char*) &temp_index_node, sizeof (ch_node));
	}
	}
}

void find_record(int key_find)
{
	hash_value=key_find%hash_size;
	long db_find_ptr=search(key_find,"find");
	if(db_find_ptr!=-1)
	{
	fp_db.seekp(db_find_ptr,ios::beg);
	fp_db.read((char*) &db_record,(sizeof (f_rec)));
	cout<<db_record.num<<" "<<(float)db_record.amt<<" "<<db_record.type<<" "<<db_record.acct<<" "<<db_record.hour<<" "<<db_record.min<<" "<<db_record.day<<" "<<db_record.year<<"\n";
	}
	else
	{
	cout<<"Record "<<key_find<<" does not exist.\n";
	}
}
void delete_record(int key_delete)
{
	hash_value=key_delete%hash_size;
	long db_delete_ptr=search(key_delete,"delete");
	if(db_delete_ptr==-1)
	{
	cout<<"Record "<<key_delete<<" does not exist.\n";
	}
	else
	{
	fp_index.seekp(db_delete_ptr,ios::beg);
	fp_index.write((char*) &temp_index_node,sizeof(ch_node));
	}
}

void write_index_init()
{
fp_index.seekp(0, ios::end);
long db_ptr;
db_ptr=fp_index.tellp();
if (db_ptr == 0 ) 
{
  index_node.k = -1;
  index_node.next = -1;
  index_node.rec = -1;
  for( int i = 0; i < 1001; i++ ) 
  {
    fp_index.write( (char *) &index_node, sizeof( ch_node ));
  }
}
}

void db_pointer_fun()
{
	fp_db.seekp(0,ios::end);
	db_pointer=fp_db.tellp();
}
void write_db_record()
{
	db_record.num=token[1];
	db_record.amt=token[2];
	db_record.type=token[3][0];
	db_record.acct=token[4];
	db_record.hour=token[5];
	db_record.min=token[6];
	db_record.day=token[7];
	db_record.year=token[8];

	fp_db.write( (char *) &db_record, sizeof(f_rec));
}

void main()
{
string input;
create_file_db("db.dat");
create_file_index("db.idx");
write_index_init();
while(1)
{

cin>>input;
input.token(token,20," ");


if(token[0]=="add")
{
	db_pointer_fun();
	add_record(atoi(token[1]));
	if(ret_pointer!=-1)
	{
	write_db_record();
	}
}

if(token[0]=="find")
{
find_record(atoi(token[1]));
}
if(token[0]=="delete")
{
delete_record(atoi(token[1]));
}
if(token[0]=="print")
{
	
	for(int k=0;k<hash_size;k++)
	{
	fp_index.seekp(k*sizeof(ch_node),ios::beg);
	fp_index.read((char*) &temp_index_node,sizeof(ch_node));
	cout<<k<<": ";
	while(1)
	{
	if(temp_index_node.k!=-1)
	{
		cout<<temp_index_node.k<<"/"<<temp_index_node.rec<<" ";
	}
	if(temp_index_node.next==-1)
	{
		break;
	}
	fp_index.seekp(temp_index_node.next,ios::beg);
	fp_index.read((char*) &temp_index_node,sizeof(ch_node));
	}
	cout<<"\n";
	
	}
}

if(token[0]=="end")
{
break;
}
}
_getch();
}