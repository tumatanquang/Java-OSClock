package uc.lang;
import java.io.File;
import java.io.FileOutputStream;
import java.io.IOException;
import java.io.InputStream;
import java.util.Locale;
public final class OSClock {
	static {
		NativeUtils.loadLibraryFromJar();
	}
	public static final native long currentTimeMillis();
	public static final native long nanoTime();
	public static final native long getEpochSecond();
	public static final native long getEpochMillis();
	private static final class NativeUtils {
		private static final void loadLibraryFromJar() {
			String os = System.getProperty("os.name").toLowerCase(Locale.ENGLISH),
			arch = System.getProperty("os.arch").toLowerCase(Locale.ENGLISH),
			mappedOs, mappedArch, ext, libPrefix;
			if(os.indexOf("win") >= 0) {
				mappedOs = "windows";
				ext = "dll";
				libPrefix = "";
			}
			else if(os.indexOf("mac") >= 0) {
				mappedOs = "macos";
				ext = "dylib";
				libPrefix = "lib";
			}
			else if(os.indexOf("nux") >= 0 || os.indexOf("nix") >= 0 || os.indexOf("aix") >= 0) {
				mappedOs = "linux";
				ext = "so";
				libPrefix = "lib";
			}
			else if(os.indexOf("android") >= 0) {
				mappedOs = "android";
				ext = "so";
				libPrefix = "lib";
			}
			else throw new RuntimeException("Unsupported OS: " + os);
			if(arch.indexOf("64") >= 0) mappedArch = "x64";
			else if(arch.equals("aarch64") || arch.equals("arm64")) mappedArch = "arm64";
			else if(arch.indexOf("arm") >= 0) mappedArch = "arm";
			else mappedArch = "x86";
			String libName = libPrefix + "osclock." + ext,
			resourcePath = "/uc/lang/native/" + mappedOs + '-' + mappedArch + '/' + libName;
			InputStream is = null;
			File temp = null;
			FileOutputStream fos = null;
			try {
				is = OSClock.class.getResourceAsStream(resourcePath);
				if(is == null) throw new UnsatisfiedLinkError("Native lib not found in JAR: " + resourcePath);
				temp = File.createTempFile("osclock", '.' + ext);
				temp.deleteOnExit();
				fos = new FileOutputStream(temp);
				byte[] buf = new byte[4096];
				for(int count; (count = is.read(buf)) > 0;) {
					fos.write(buf, 0, count);
				}
				fos.close();
				fos = null;
				is.close();
				is = null;
				System.load(temp.getAbsolutePath());
			}
			catch(Exception e) {
				if(temp != null) temp.delete();
				throw new RuntimeException("Failed to load native lib: " + resourcePath, e);
			}
			finally {
				if(fos != null) {
					try {
						fos.close();
					}
					catch(IOException ignored) {}
					fos = null;
				}
				if(is != null) {
					try {
						is.close();
					}
					catch(IOException ignored) {}
					is = null;
				}
			}
		}
	}
}