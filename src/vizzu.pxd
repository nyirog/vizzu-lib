cdef extern from "apps/weblib/interface.cpp":
    pass


cdef extern from "apps/weblib/interface.h" namespace "Vizzu":
    cdef cppclass Interface:
        Interface() except +
        const char *version() const;
        void init();
        void setLogging(bint enable);
        void keyPress(int key, bint ctrl, bint alt, bint shift);
        void mouseMove(double x, double y);
        void mouseDown(double x, double y);
        void mouseUp(double x, double y);
        void mouseLeave();
        void update(double scale, double width, double height, bint force);
        void poll();

        void *storeChart();
        void restoreChart(void *chart);
        void freeChart(void *chart);
        const char *getStyleList();
        const char *getStyleValue(const char *path);
        void setStyleValue(const char *path, const char *value);
        const char *getChartParamList();
        const char *getChartValue(const char *path);
        void setChartValue(const char *path, const char *value);
        void setChartFilter(bint (*filter)(const void *));
        void addDimension(const char *name, const char **categories, int count);
        void addMeasure(const char *name, double *values, int count);
        void addRecord(const char **cells, int count);
        int addEventListener(const char *name);
        void removeEventListener(const char *name, int id);
        void preventDefaultEvent();
        void animate(void (*callback)());
        void animControl(const char *command, const char *param);
        void setAnimValue(const char *path, const char *value);


