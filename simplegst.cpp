#include <gst\gst.h>
#include <gst\interfaces\xoverlay.h>

#include <QApplication>
#include <QTimer>
#include <QWidget>

WId m_xwinid;
GstElement *m_pipeline;
GstElement *m_sink;

// If we ever want to handle messages
static gboolean HandleMessage(GstBus *bus, GstMessage *msg, gpointer data)
{
    if (msg)
    {
        switch (GST_MESSAGE_TYPE(msg))
        {
        case GST_MESSAGE_ERROR:
        case GST_MESSAGE_WARNING:
        case GST_MESSAGE_INFO:
        case GST_MESSAGE_EOS:
        case GST_MESSAGE_DURATION:
        case GST_MESSAGE_STATE_CHANGED:
        case GST_MESSAGE_STREAM_STATUS:
        case GST_MESSAGE_UNKNOWN:
        case GST_MESSAGE_TAG:
        case GST_MESSAGE_BUFFERING:
        case GST_MESSAGE_STATE_DIRTY:
        case GST_MESSAGE_STEP_DONE:
        case GST_MESSAGE_CLOCK_PROVIDE:
        case GST_MESSAGE_CLOCK_LOST:
        case GST_MESSAGE_NEW_CLOCK:
        case GST_MESSAGE_STRUCTURE_CHANGE:
        case GST_MESSAGE_APPLICATION:
        case GST_MESSAGE_ELEMENT:
        case GST_MESSAGE_SEGMENT_START:
        case GST_MESSAGE_SEGMENT_DONE:
        case GST_MESSAGE_LATENCY:
        case GST_MESSAGE_ASYNC_START:
        case GST_MESSAGE_ASYNC_DONE:
        case GST_MESSAGE_REQUEST_STATE:
        case GST_MESSAGE_STEP_START:
        case GST_MESSAGE_QOS:
        case GST_MESSAGE_PROGRESS:
        default:
            break;
        }
    }
    return TRUE;
}

int main(int argc, char *argv[])
{
    if (!g_thread_supported())
        g_thread_init(NULL);

    gst_init(&argc, &argv);
    QApplication app(argc, argv);
    app.connect(&app, SIGNAL(lastWindowClosed()), &app, SLOT(quit()));

    m_pipeline = gst_element_factory_make("playbin2", NULL);
    g_object_set(m_pipeline, "uri", "http://docs.gstreamer.com/media/sintel_cropped_multilingual.webm", NULL);
    m_sink = gst_element_factory_make("xvimagesink", NULL);

    QWidget window;
    window.resize(640, 480);
    window.show();
    m_xwinid = window.winId();

    gst_x_overlay_set_window_handle(GST_X_OVERLAY(m_pipeline), m_xwinid);

    GstStateChangeReturn sret = gst_element_set_state(m_pipeline,
        GST_STATE_PLAYING);
    if (sret == GST_STATE_CHANGE_FAILURE) {
        gst_element_set_state(m_pipeline, GST_STATE_NULL);
        gst_object_unref(m_pipeline);
        // Exit application
        QTimer::singleShot(0, QApplication::activeWindow(), SLOT(quit()));
    }

    int ret = app.exec();

    window.hide();
    gst_element_set_state(m_pipeline, GST_STATE_NULL);
    gst_object_unref(m_pipeline);

    return ret;
}
