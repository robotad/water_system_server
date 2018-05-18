package com.robotad.waterserver.filter;

import javax.servlet.ServletOutputStream;
import javax.servlet.http.HttpServletResponse;
import javax.servlet.http.HttpServletResponseWrapper;
import java.io.ByteArrayOutputStream;
import java.io.IOException;

public class BufferedResponseWrapper extends HttpServletResponseWrapper {
    private ByteArrayOutputStream bos;
    private HttpServletResponse original;
    private SplitServletOutputStream ss_os = null;

    public BufferedResponseWrapper(HttpServletResponse response) {
        super(response);
        original = response;
    }

    public ServletOutputStream getOutputStream() throws IOException {
        prepOutputStream();
        return ss_os;
    }

    private void prepOutputStream() throws IOException {
        if (ss_os == null) {
            bos = new ByteArrayOutputStream();
            ss_os = new SplitServletOutputStream(original.getOutputStream(), bos);
        }
    }

    public String getContent() throws IOException {
        prepOutputStream();
        return bos.toString();
    }
}
