package com.robotad.waterserver.filter;

import javax.servlet.ReadListener;
import javax.servlet.ServletInputStream;
import java.io.ByteArrayInputStream;

public class BufferedServletInputStream extends ServletInputStream {
    private ByteArrayInputStream ba_is;

    public BufferedServletInputStream(ByteArrayInputStream ba_is) {
        this.ba_is = ba_is;
    }

    @Override
    public int read() {
        return ba_is.read();
    }

    @Override
    public boolean isFinished() {
        return ba_is.available() == 0;
    }

    @Override
    public boolean isReady() {
        return true;
    }

    @Override
    public void setReadListener(ReadListener listener) {
        throw new RuntimeException("Not implemented");
    }
}
