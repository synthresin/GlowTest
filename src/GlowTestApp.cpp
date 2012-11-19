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
	
    gl::clear( Color( 0, 0, 0 ) );
    
    
    // 일단 Fbo 를 하나 만들어서 그린다. 드로잉 컨텍스트가 완전 새로운게 되는듯? Fbo 에 대해 더 공부해볼 필요가 있당...
    // 뭔가 카메라 설정도 마구마구 해서    
    
    // 뒤집힘 해결 테스트
    mCam.lookAt(Vec3f(0,0,500.0f), Vec3f::zero(), Vec3f::yAxis() );
    
    mNormalFbo.bindFramebuffer();
        gl::setMatrices(mCam);
        gl::clear( ColorA(0, 0, 0, 0 ) );
        gl::color(0.4, 0.2, 0.5);
        gl::drawSphere(Vec3f(0,0, 0), 50.0);
        gl::drawSphere(Vec3f(100,100, 0), 50.0);
        gl::color(1.0, 0.0, 0.0, 1.0);
        gl::drawSphere(Vec3f(0,-100, 0), 50.0);
        gl::drawSphere(Vec3f(-100,-100, 0), 50.0);
    mNormalFbo.unbindFramebuffer();
    
    mGlowFbo.bindFramebuffer();
        gl::setMatrices(mCam);
        gl::clear( ColorA(0, 0, 0, 0 ) );
        gl::color(0.4, 0.2, 0.5);
        gl::drawSphere(Vec3f(0,0, 0), 50.0);
        gl::drawSphere(Vec3f(100,100, 0), 50.0);
        gl::color(1.0, 0.0, 0.0, 1.0);
        gl::drawSphere(Vec3f(0,-100, 0), 50.0);
        gl::drawSphere(Vec3f(-100,-100, 0), 50.0);
    mGlowFbo.unbindFramebuffer();
    
    // 텍스쳐 블러시킬 Fbo 를 만든다. 텍스쳐가 정면에 나와야 되니까 카메라 세팅을 신더 기본으로 해준다.
    // 블러 때리는용 Fbo 를 텍스쳐화 시켜 그린다.
    mBlurFbo.bindFramebuffer();
        gl::clear( ColorA(0, 0, 0, 0 ) );
        gl::setMatricesWindow(getWindowSize(), false);
        mGlowFbo.getTexture().bind(1);  // 얘를 바인딩 해야한다. 그래서 sampler 로 전달하구, 텍스쳐로 그려내야돼.
    
        mBlurShader.bind();
        mBlurShader.uniform("GlowFboTex", 1);   // glowfbo 를 전사한 텍스쳐를 sampler1d 로 보낸다.
        mBlurShader.uniform("sampleOffset", Vec2f(0.7, 0.3) * ( 3.0f / getWindowWidth() ) );
        gl::drawSolidRect( Rectf(0.0, 0.0, getWindowWidth(), getWindowHeight() ) );
        mBlurShader.unbind();
    
        mGlowFbo.getTexture().unbind();
    mBlurFbo.unbindFramebuffer();
    
    mMultiplyFbo.bindFramebuffer();
    gl::clear( ColorA(0, 0, 0, 0 ) );
    gl::setMatricesWindow(getWindowSize(), false);
    // 두가지 합칠 소스 가져오기
    mNormalFbo.getTexture().bind(1);
    mBlurFbo.getTexture().bind(2);
    
    mMultiShader.bind();
    mMultiShader.uniform("Tex1", 1);
    mMultiShader.uniform("Tex2", 2);
    gl::drawSolidRect( Rectf(0.0, 0.0, getWindowWidth(), getWindowHeight() ) );
    mMultiShader.unbind();
    mMultiplyFbo.unbindFramebuffer();

    
    
    
    
    //gl::draw(mMultiplyFbo.getTexture());
    gl::color(1.0, 1.0, 1.0, 1.0);
    gl::setMatricesWindow(getWindowSize() , false);
    //gl::draw(mNormalFbo.getTexture());
    //gl::draw(mGlowFbo.getTexture());
    //gl::draw(mBlurFbo.getTexture());
    gl::draw(mMultiplyFbo.getTexture());
    
    
    

}

CINDER_APP_BASIC( GlowTestApp, RendererGl )
