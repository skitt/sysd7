int uidcomp(char *uid1,char *uid2)
{
  int i;
  for(i=0;i<6;i++) if(uid1[i] != uid2[i]) break;
  if(i==6) return 1; else return 0;
}

void main()
{ 
  printf("Result = %i\n",uidcomp("123414","123414"));
}
