package main;

import java.util.HashMap;
import java.util.HashSet;

public class GlobalVars {
	static boolean hasTimer = false;
	static boolean isTCP =false;
	static boolean enableOpenNF = false;
	static String OpenNFState = "packet_counter";
	static boolean enalbeSGX = false;
	static boolean useDPDK = false;
	static boolean enableGPU = false;
	static HashSet<String> flags = new HashSet<String>();
	static HashMap<String, String> values = new HashMap<>();
}
