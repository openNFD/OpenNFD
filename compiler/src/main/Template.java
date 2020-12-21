package main;

import java.io.File;
import java.io.FileInputStream;
import java.io.FileOutputStream;
import java.io.FileWriter;
import java.io.IOException;
import java.nio.channels.FileChannel;
import java.util.HashMap;
import java.util.HashSet;
import java.util.regex.Matcher;
import java.util.regex.Pattern;

public class Template {
	private FileWriter fw1;
	private File f1;
	private File f2;
	private File f3;
	private File f4;
	private File folder;
	private File f5;
	private File f6;
	private File f7;

	public Template() throws IOException {

		f1 = new File("template/basic_classes.cpp");
		f2 = new File("template/basic_classes.h");
		f3 = new File("template/decode.h");
		f4 = new File("template/basic_methods.cpp");
		folder = new File("NF/");
		if (GlobalVars.useDPDK) {
			f5 = new File("template/dpdk/Makefile");
		}else {
			f5 = new File("template/pcap/makefile");
		}
		
		
		f6 = new File("template/TCPClient.cpp");
		f7 = new File("template/TCPClient.h");
		copyFile(f1, folder);
		copyFile(f2, folder);
		copyFile(f3, folder);
		copyFile(f4, folder);
		copyFile(f5, folder);
		if (GlobalVars.isTCP) {
			copyFile(f6, folder);
			copyFile(f7, folder);
		}
		if (GlobalVars.enableOpenNF) {
			copyFile(new File("template/OpenNF/basic_classes.h"), folder);
			copyFile(new File("template/basic_classes.cpp"), folder);
			copyFile(new File("template/basic_methods.cpp"), folder);
			copyFile(new File("template/decode.h"), folder);
			copyFile(new File("template/OpenNF/global.h"), folder);
			copyFile(new File("template/OpenNF/global.cpp"), folder);
			copyFile(new File("template/OpenNF/SDMBNLocal.h"), folder);
			copyFile(new File("template/OpenNF/serialize.cpp"), folder);
			copyFile(new File("template/OpenNF/serialize.h"), folder);
			copyFile(new File("template/OpenNF/makefile"), folder);
		}
		fw1 = new FileWriter(new File("NF/basic_classes.cpp"), true);
	}

	public void CloseAll() throws IOException {
		fw1.flush();
		fw1.close();

	}

	public void DumpTemplate(HashSet<String> write, HashSet<String> read) throws IOException {
		fw1.write("Flow::Flow(u_char * packet, int totallength)  {\n");
		if (read.contains("tag")) {
			fw1.write("    this->headers[TAG] = new int(0);\n");
		}
		if (read.contains("iplen")) {
			fw1.write("    this->headers[IPLEN] = new int(totallength);\n");
		}
		if (read.contains("sip") || read.contains("dip") || read.contains("UDP") || read.contains("sport")
				|| read.contains("sport") || read.contains("flag_fin") || read.contains("flag_syn")
				|| read.contains("flag_ack")) {
			fw1.write("    this->pkt = packet;\n" + "    int ethernet_header_length = 14;\n" + "\n"
					+ "    EtherHdr* e_hdr = (EtherHdr*) packet;\n" + "    if ( ntohs(e_hdr->ether_type) == 0x8100)	\n"
					+ "	    ethernet_header_length = 14+4; \n" + "    else\n" + "        ethernet_header_length = 14;\n");

			fw1.write("    IPHdr * ip_hdr = (IPHdr*) (packet+ethernet_header_length);\n");
			if (read.contains("sip")) {
				fw1.write("    int src_addr = ntohl(ip_hdr->ip_src.s_addr);\n"
						+ "	this->headers[SIP] = new IP(src_addr, 32);\n");
			}
			if (read.contains("dip")) {
				fw1.write("    int dst_addr = ntohl(ip_hdr->ip_dst.s_addr);\n"
						+ "	this->headers[SIP] = new IP(dst_addr, 32);\n");
			}
			if (read.contains("UDP") || read.contains("TCP")) {
				fw1.write("    short protocol = (short)ntohs(ip_hdr->ip_proto);\n");
				if (read.contains("UDP")) {
					fw1.write("    this->headers[UDP] = new int((protocol==IPPROTO_UDP)?1:0);\n");
				}
				if (read.contains("TCP")) {
					fw1.write("    this->headers[TCP] = new int((protocol==IPPROTO_TCP)?1:0);\n");
				}
			}
			if (read.contains("sport") || read.contains("sport") || read.contains("flag_fin")
					|| read.contains("flag_syn") || read.contains("flag_ack")) {
				fw1.write("    int ip_header_length = int(ip_hdr->ip_hlen) * 4;\n"
						+ "    TCPHdr *tcph =(TCPHdr *)(packet+ethernet_header_length+ip_header_length);\n");
				if (read.contains("sport"))
					fw1.write("    this->headers[SPORT] = new int(ntohs(tcph->th_sport));\n");
				if (read.contains("dport"))
					fw1.write("    this->headers[DPORT] = new int(ntohs(tcph->th_dport));\n");
				if (read.contains("flag_fin"))
					fw1.write("    this->headers[FLAG_FIN] = new int(ntohs(tcph->th_flags) & TH_FIN);\n");
				if (read.contains("flag_syn"))
					fw1.write("    this->headers[FLAG_SYN] = new int(ntohs(tcph->th_flags) & TH_SYN);\n");
				if (read.contains("flag_ack"))
					fw1.write("    this->headers[FLAG_ACK] = new int(ntohs(tcph->th_flags) & TH_ACK);\n");
			}
		}
		fw1.write("}\n");

		fw1.write("void Flow::clean() {	\n");

		if (write.size() > 0) {
			fw1.write("    u_char * packet = this->pkt;\n"
					+ "    int ethernet_header_length = 14; \n" + 
					"    EtherHdr* e_hdr = (EtherHdr*) this->pkt;\n" + 
					"    if ( ntohs(e_hdr->ether_type) == 0x8100)	\n" + 
					"	    ethernet_header_length = 14+4; \n" + 
					"    else\n" + 
					"        ethernet_header_length = 14;\n");
			if (write.contains("sip") || write.contains("dip") || write.contains("sport") || write.contains("dport")) {
				fw1.write("    IPHdr * ip_hdr = (IPHdr*) (packet+ethernet_header_length);\n");
				if (write.contains("sip")) {
					fw1.write("    ip_hdr->ip_src.s_addr = htonl( ((IP *)this->headers[SIP])->ip );\n");
				}
				if (write.contains("dip")) {
					fw1.write("    ip_hdr->ip_dst.s_addr = htonl( ((IP *)this->headers[DIP])->ip );\n");
				}
				if (write.contains("sport") || write.contains("dport")) {
					fw1.write("    int ip_header_length = int(ip_hdr->ip_hlen) * 4;\n" + 
							"	TCPHdr *tcph =(TCPHdr *)(packet+ethernet_header_length+ip_header_length);\n");
					if (write.contains("sport")) {
						fw1.write("    tcph->th_sport =htons(u_short(*((int*)this->headers[SPORT])));\n");
					}
					if (write.contains("dport")) {
						fw1.write("    tcph->th_dport =htons(u_short(*((int*)this->headers[DPORT])));\n");
					}
				}
			}

		}
		fw1.write("}\n");

		CloseAll();
	}

	public static boolean copyFile(File srcFile, File dstDir) {
		if (!srcFile.exists() || srcFile.isDirectory()) {
			return false;
		}
		if (!dstDir.exists()) {
			dstDir.mkdirs();
		}
		String oldFileName = srcFile.getName();
		Pattern suffixPattern = Pattern.compile("\\.\\w+");
		Matcher matcher = suffixPattern.matcher(oldFileName);
		String nameBody;
		String suffix;
		if (matcher.find()) {
			nameBody = oldFileName.substring(0, matcher.start());
			suffix = oldFileName.substring(matcher.start());
		} else {
			nameBody = oldFileName;
			suffix = "";
		}
		int fileNumber = 0;
		File newFile = new File(dstDir, oldFileName);

		try {
			FileChannel fileIn = new FileInputStream(srcFile).getChannel();
			FileChannel fileOut = new FileOutputStream(newFile).getChannel();
			fileIn.transferTo(0, fileIn.size(), fileOut);
			fileIn.close();
			fileOut.close();
		} catch (IOException e) {
			return false;
		}
		return true;
	}
}
