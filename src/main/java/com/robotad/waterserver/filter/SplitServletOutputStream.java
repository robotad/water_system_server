package com.robotad.waterserver.filter;

import org.apache.commons.io.output.TeeOutputStream;

import javax.servlet.ServletOutputStream;
import javax.servlet.WriteListener;
import java.io.IOException;
import java.io.OutputStream;

public class SplitServletOutputStream extends ServletOutputStream {
    private TeeOutputStream tos;

    public SplitServletOutputStream(OutputStream one, OutputStream two) {
        this.tos = new TeeOutputStream(one, two);
    }

    @Override
    public void write(int arg0) throws IOException {
        this.tos.write(arg0);
    }

    @Override
    public boolean isReady() {
        return true;
    }

    @Override
    public void setWriteListener(WriteListener var1) {
        throw new RuntimeException("Not implemented");
    }
}
