package com.robotad.waterserver.filter;

import javax.servlet.ServletInputStream;
import javax.servlet.http.HttpServletRequest;
import javax.servlet.http.HttpServletRequestWrapper;
import java.io.*;

public class BufferedRequestWrapper extends HttpServletRequestWrapper {
    private byte[] buffer;

    public BufferedRequestWrapper(HttpServletRequest request) throws IOException {
        super(request);

        InputStream is = request.getInputStream();
        ByteArrayOutputStream ba_os = new ByteArrayOutputStream();
        byte[] buffer_ = new byte[1024];
        int num_bytes_read;
        while ((num_bytes_read = is.read(buffer_)) > 0) {
            ba_os.write(buffer_, 0, num_bytes_read);
        }
        this.buffer = ba_os.toByteArray();
    }

    public String getRequestBody(int length) throws IOException {
        BufferedReader reader = new BufferedReader(new InputStreamReader(getInputStream()));
        String buf_str = bufToString(reader, length, 1024);
        reader.close();
        return buf_str;
    }

    public static String bufToString(BufferedReader reader, int length, int buf_size) throws IOException {
        int num_read;
        StringBuilder sb = new StringBuilder();

        char[] buf = new char[buf_size];
        while (length > 0 && // Read from the buffer (length = how much left to read)
                (num_read = reader.read(buf, 0, length > buf_size ? buf_size : length)) > 0) {
            sb.append(new String(buf, 0, num_read)); // Append read string to sb
            length -= num_read; // Decrease length
        }
        return sb.toString();
    }

    @Override
    public ServletInputStream getInputStream() {
        return new BufferedServletInputStream(new ByteArrayInputStream(this.buffer));
    }
}
