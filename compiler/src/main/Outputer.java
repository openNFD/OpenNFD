package main;

import java.io.File;
import java.io.FileInputStream;
import java.io.FileNotFoundException;
import java.io.FileReader;
import java.io.FileWriter;
import java.io.IOException;
import java.io.InputStream;
import java.io.InputStreamReader;
import java.io.Reader;

import symbols.SymbolTable;

public class Outputer {
	static FileWriter fw;
	static StringBuffer s0;
	static StringBuffer s1;
	static StringBuffer s2;

	static void write(String s) {
		s1.append(s);
	}

	static void write(File f) {
		Reader reader = null;
		try {
			reader = new InputStreamReader(new FileInputStream(f));
			int c;
			while ((c = reader.read()) != -1) {
				s1.append((char) c);
			}
			reader.close();
		} catch (FileNotFoundException e) {
			// TODO Auto-generated catch block
			e.printStackTrace();
		} catch (IOException e) {
			// TODO Auto-generated catch block
			e.printStackTrace();
		}

	}

	static void writen(String s) {
		s1.append(s + "\n");
	}

	static void pre_write(String s) {
		s0.append(s + "\n");
	}

	public Outputer(String filename) {
		try {
			this.s0 = new StringBuffer();
			this.s1 = new StringBuffer();
			this.s2 = new StringBuffer();

			Outputer.fw = new FileWriter(new File(filename));

		} catch (IOException e) {
			// TODO Auto-generated catch block
			e.printStackTrace();
		}
	}

	private void dumpFromFile(String filename) {
		int c;
		Reader reader;
		try {
			reader = new InputStreamReader(new FileInputStream(new File(filename)));
			while ((c = reader.read()) != -1) {
				fw.write(c);
			}
			fw.flush();
			reader.close();
		} catch (IOException e) {
			// TODO Auto-generated catch block
			e.printStackTrace();
		}

	}

	public void close() {
		try {
			if (GlobalVars.isTCP) {
				dumpFromFile("template/slice5.txt");
			} else {
				if (GlobalVars.enableOpenNF) {
					dumpFromFile("template/OpenNF/slice1-1.txt");
				} else {
					if (GlobalVars.useDPDK) {
						dumpFromFile("template/dpdk/slice1-1.txt");
					}else {
						if (GlobalVars.enableGPU) {
							dumpFromFile("template/GPU/slice1-1.txt");
						}else {
							dumpFromFile("template/pcap/slice1-1.txt");
						}
					}
				}
			}
			if (GlobalVars.flags.contains("encrypt")){
				if (GlobalVars.enableGPU){
					Outputer.fw.write("extern \"C\" int gpu_encrypt (char *akey, char *plain, unsigned long length,char* cipher);\n");
				}else{
					Outputer.fw.write("extern  \"C\" int cpu_encrypt(char* key, char *buf, unsigned long length,char *cipher);\n");
				}
				Outputer.fw.write("char *key = " +GlobalVars.values.get("key")+";\n");
			}


			if (GlobalVars.enableOpenNF) {
				dumpFromFile("template/OpenNF/slice1-2.txt");
			} else {
				if (GlobalVars.useDPDK) {
					dumpFromFile("template/dpdk/slice1-2.txt");
				}else {
					if (GlobalVars.enableGPU) {
						dumpFromFile("template/GPU/slice1-2.txt");
					}else {
						dumpFromFile("template/pcap/slice1-2.txt");
					}
				}
			}

			/* main section */
			Outputer.fw.write(s0.toString());
			Outputer.fw.write(s1.toString());
			Outputer.fw.write(s2.toString());
			fw.flush();

			if (GlobalVars.isTCP == false) {
				if (GlobalVars.enableOpenNF) {
					dumpFromFile("template/OpenNF/slice2.txt");
				} else {
					if (GlobalVars.useDPDK) {
						dumpFromFile("template/dpdk/slice2.txt");
					}else {
						dumpFromFile("template/pcap/slice2.txt");
					}
				}
			}

			Outputer.fw.close();
		} catch (IOException e) {
			// TODO Auto-generated catch block
			e.printStackTrace();
		}
	}

}
