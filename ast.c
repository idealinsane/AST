int main(){
	return 	main1();
}
char* my_realloc(char* old, int oldlen, int newlen){
	return ;
}
void error(){
	exit(1);
}
void takechar(){
}
void get_token(){
}
int peek(char* s){
	return ;
}
int accept(char* s){
}
void expect(char* s){
}
void save_int(char* p, int n){
}
int load_int(char* p){
	return ;
}
void emit(int n, char* s){
}
void be_push(){
	emit(1, "\x50");
}
void be_pop(int n){
	emit(6, "\x81\xc4....");
	save_int(, );
}
int sym_lookup(char* s){
	return ;
}
void sym_declare(char* s, int type, int value){
	save_int(, );
}
int sym_declare_global(char* s){
	return ;
}
void sym_define_global(int current_symbol){
	save_int(, );
}
void sym_get_value(char* s){
	emit(5, "\xb8....");
	save_int(, );
}
void be_start(){
	emit(16, "\x7f\x45\x4c\x46\x01\x01\x01\x00\x00\x00\x00\x00\x00\x00\x00\x00");
	emit(16, "\x02\x00\x03\x00\x01\x00\x00\x00\x54\x80\x04\x08\x34\x00\x00\x00");
	emit(16, "\x00\x00\x00\x00\x00\x00\x00\x00\x34\x00\x20\x00\x01\x00\x00\x00");
	emit(16, "\x00\x00\x00\x00\x01\x00\x00\x00\x00\x00\x00\x00\x00\x80\x04\x08");
	emit(16, "\x00\x80\x04\x08\x10\x4b\x00\x00\x10\x4b\x00\x00\x07\x00\x00\x00");
	emit(16, "\x00\x10\x00\x00\xe8\x00\x00\x00\x00\x89\xc3\x31\xc0\x40\xcd\x80");
	sym_define_global();
	emit(7, "\x5b\x5b\x31\xc0\x40\xcd\x80");
	sym_define_global();
	emit(10, "\xb8\x03\x00\x00\x00\x31\xdb\x53\x89\xe1");
	emit(10, "\x31\xd2\x42\xcd\x80\x85\xc0\x58\x75\x05");
	emit(6, "\xb8\xff\xff\xff\xff\xc3");
	sym_define_global();
	emit(4, "\x8b\x44\x24\x04");
	emit(10, "\x50\x31\xdb\xb8\x2d\x00\x00\x00\xcd\x80");
	emit(10, "\x5b\x01\xc3\x50\x53\xb8\x2d\x00\x00\x00");
	emit(8, "\xcd\x80\x5b\x39\xc3\x58\x74\x05");
	emit(6, "\xb8\xff\xff\xff\xff\xc3");
	sym_define_global();
	emit(8, "\xb8\x04\x00\x00\x00\x31\xdb\x43");
	emit(9, "\x8d\x4c\x24\x04\x89\xda\xcd\x80\xc3");
	save_int(, );
}
void be_finish(){
	save_int(, );
	save_int(, );
}
void promote(int type){
}
int primary_expr(){
	get_token();
	return ;
}
void binary1(int type){
	promote();
	be_push();
}
int binary2(int type, int n, char* s){
	promote();
	emit(, );
	return 3;
}
int postfix_expr(){
	return ;
}
int additive_expr(){
}
int shift_expr(){
}
int relational_expr(){
	return ;
}
int equality_expr(){
}
int bitwise_and_expr(){
	return ;
}
int bitwise_or_expr(){
	return ;
}
int expression(){
	return ;
}
void type_name(){
	get_token();
}
void statement(){
}
void program(){
}
int main1(){
	be_start();
	get_token();
	program();
	be_finish();
	return 0;
}
