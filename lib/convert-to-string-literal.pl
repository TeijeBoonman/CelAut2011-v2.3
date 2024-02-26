#!/opt/local/bin/perl

print "const char *dxf_header = \"\\\n";

while (<>) {
  chomp;
  print "$_\\n\\\n";
}
print "\";\n";

