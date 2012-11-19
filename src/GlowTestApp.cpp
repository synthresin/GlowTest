#include "cinder/app/AppBasic.h"
#include "cinder/gl/gl.h"
#include "cinder/gl/Fbo.h"
#include "cinder/gl/GlslProg.h"
#include "cinder/Camera.h"

using namespace ci;
using namespace ci::app;
using namespace std;

class GlowTestApp : public AppBasic {
  public:
    void prepareSettings( Settings * settings);
	void setup();
	void mouseDown( MouseEvent event );	
	void update();
	void draw();
    CameraPersp mCam;
    
    
    gl::Fbo mNormalFbo;     // 정상적인 화면 출력을 위한 Fbo
    gl::Fbo mGlowFbo;
    gl::Fbo mBlurFbo;
    gl::Fbo mMultiplyFbo;
    
    gl::GlslProg    mBlurShader;
    gl::GlslProg    mMultiShader;
};

void GlowTestApp::prepareSettings(Settings * settings)
{
    settings->setWindowSize(1280, 720);
}

void GlowTestApp::setup()
{
    mNormalFbo = gl::Fbo(1280, 720);
    mGlowFbo = gl::Fbo(1280, 720);
    mBlurFbo = gl::Fbo(1280, 720);
    mMultiplyFbo = gl::Fbo(1280, 720);
    
    try {
        mBlurShader = gl::GlslProg(loadResource("vert.glsl"),loadResource("blur.glsl")); // 셰이더 소스 넣어야함.
    } catch ( gl::GlslProgCompileExc &exc ) {
        std::cout << "error : " << exc.what() << std::endl;
    } catch (...) {
        std::cout << "unknown error." << std::endl;
    }
    
    try {
        mMultiShader = gl::GlslProg(loadResource("vert.glsl"),loadResource("multi.glsl")); // 셰이더 소스 넣어야함.
    } catch ( gl::GlslProgCompileExc &exc ) {
        std::cout << "error : " << exc.what() << std::endl;
    } catch (...) {
        std::cout << "unknown error." << std::endl;
    }
    
    gl::enableAlphaBlending();
    gl::enableAlphaTest();
    
    mCam.setPerspective(75, getWindowAspectRatio(), 5.0, 5000.0f);
}

void GlowTestApp::mouseDown( MouseEvent event )
{
}

void GlowTestApp::update()
{
    
}

void GlowTestApp::draw()
{
	// clear out the window with black
	
    gl::clear( Color( 1, 0, 0 ) );
    
    
    // 일단 Fbo 를 하나 만들어서 그린다. 드로잉 컨텍스트가 완전 새로운게 되는듯? Fbo 에 대해 더 공부해볼 필요가 있당...
    // 뭔가 카메라 설정도 마구마구 해서    
    
    // 뒤집힘 해결 테스트
    mCam.lookAt(Vec3f(0,0,500.0f), Vec3f::zero(), Vec3f::yAxis() );
    gl::pushMatrices();
    gl::setMatrices(mCam);
    gl::color(0.4, 0.2, 0.5);
    gl::drawSphere(Vec3f(0,0, 0), 50.0);
    gl::drawSphere(Vec3f(100,100, 0), 50.0);
    gl::popMatrices();
    
    mNormalFbo.bindFramebuffer();
    gl::setMatrices(mCam);
    gl::clear( ColorA(0, 0, 0, 0 ) );
    gl::color(1.0, 1.0, 1.0, 1.0);
    gl::drawSphere(Vec3f(0,-100, 0), 50.0);
    gl::drawSphere(Vec3f(-100,-100, 0), 50.0);
    mNormalFbo.unbindFramebuffer();
    
    gl::setMatricesWindow(getWindowSize(), false);
    gl::draw(mNormalFbo.getTexture());
    
    

}

CINDER_APP_BASIC( GlowTestApp, RendererGl )
