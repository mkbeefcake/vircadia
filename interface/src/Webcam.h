//
//  Webcam.h
//  interface
//
//  Created by Andrzej Kapolka on 6/17/13.
//  Copyright (c) 2013 High Fidelity, Inc. All rights reserved.
//

#ifndef __interface__Webcam__
#define __interface__Webcam__

#include <QMetaType>
#include <QObject>
#include <QThread>
#include <QVector>

#include <glm/glm.hpp>
#include <glm/gtc/quaternion.hpp>

#include <opencv2/opencv.hpp>

#ifdef HAVE_OPENNI
    #include <XnCppWrapper.h>
#endif

#include "InterfaceConfig.h"

class QImage;

struct CvCapture;

class FrameGrabber;
class Joint;

typedef QVector<Joint> JointVector;
typedef std::vector<cv::KeyPoint> KeyPointVector;

class Webcam : public QObject {
    Q_OBJECT
    
public:
    
    Webcam();
    ~Webcam();

    bool isActive() const { return _active; }
    
    GLuint getColorTextureID() const { return _colorTextureID; }
    GLuint getDepthTextureID() const { return _depthTextureID; }
    const cv::Size2f& getTextureSize() const { return _textureSize; }
    const KeyPointVector& getKeyPoints() const { return _keyPoints; }
    
    const cv::RotatedRect& getEstimatedFaceRect() const { return _estimatedFaceRect; }
    const glm::vec3& getEstimatedPosition() const { return _estimatedPosition; }
    const glm::vec3& getEstimatedRotation() const { return _estimatedRotation; }
    const JointVector& getEstimatedJoints() const { return _estimatedJoints; }

    void reset();
    void renderPreview(int screenWidth, int screenHeight);

public slots:
    
    void setEnabled(bool enabled);
    void setFrame(const cv::Mat& color, int format, const cv::Mat& depth, const cv::Mat& depthPreview,
        const cv::RotatedRect& faceRect, const KeyPointVector& keyPoints, const JointVector& joints);
    
private:
    
    QThread _grabberThread;
    FrameGrabber* _grabber;
    
    bool _enabled;
    bool _active;
    GLuint _colorTextureID;
    GLuint _depthTextureID;
    cv::Size2f _textureSize;
    cv::RotatedRect _faceRect;
    cv::RotatedRect _initialFaceRect;
    KeyPointVector _keyPoints;
    JointVector _joints;
    
    uint64_t _startTimestamp;
    int _frameCount;
    
    uint64_t _lastFrameTimestamp;
    
    cv::RotatedRect _estimatedFaceRect;
    glm::vec3 _estimatedPosition;
    glm::vec3 _estimatedRotation;
    JointVector _estimatedJoints;
};

class FrameGrabber : public QObject {
    Q_OBJECT
    
public:
    
    FrameGrabber();
    virtual ~FrameGrabber();

public slots:
    
    void reset();
    void shutdown();
    void grabFrame();
    
private:
    
    bool init();
    void updateHSVFrame(const cv::Mat& frame, int format);
    
    bool _initialized;
    CvCapture* _capture;
    cv::CascadeClassifier _faceCascade;
    cv::Mat _hsvFrame;
    cv::Mat _grayFrame;
    cv::Mat _mask;
    cv::SparseMat _histogram;
    cv::Mat _backProject;
    cv::Rect _searchWindow;
    cv::Mat _grayDepthFrame;
    double _depthOffset;
    
#ifdef HAVE_OPENNI
    xn::Context _xnContext;
    xn::DepthGenerator _depthGenerator;
    xn::ImageGenerator _imageGenerator;
    xn::UserGenerator _userGenerator;
    xn::DepthMetaData _depthMetaData;
    xn::ImageMetaData _imageMetaData;
    XnUserID _userID;
#endif
};

class Joint {
public:
    
    Joint(const glm::vec3& position, const glm::quat& rotation, const glm::vec3& projected);
    Joint();
    
    bool isValid;
    glm::vec3 position;
    glm::quat rotation;
    glm::vec3 projected;
};

Q_DECLARE_METATYPE(JointVector)
Q_DECLARE_METATYPE(KeyPointVector)
Q_DECLARE_METATYPE(cv::Mat)
Q_DECLARE_METATYPE(cv::RotatedRect)

#endif /* defined(__interface__Webcam__) */
