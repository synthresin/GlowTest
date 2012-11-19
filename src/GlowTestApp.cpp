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
    
    gl::GlslProg    mBlurShader;
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
    
    try {
        mBlurShader = gl::GlslProg(loadResource("vert.glsl"),loadResource("blur.glsl")); // 셰이더 소스 넣어야함.
    } catch ( gl::GlslProgCompileExc &exc ) {
        std::cout << "error : " << exc.what() << std::endl;
    } catch (...) {
        std::cout << "unknown error." << std::endl;
    }
    gl::enableAlphaBlending();
    gl::enableAlphaTest();
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
	
    gl::setMatricesWindowPersp(getWindowSize());
    gl::clear( Color( 0, 0, 1 ) );
    
    // 일단 Fbo 를 하나 만들어서 그린다. 드로잉 컨텍스트가 완전 새로운게 되는듯? Fbo 에 대해 더 공부해볼 필요가 있당...
    // 뭔가 카메라 설정도 마구마구 해서    
    
    mNormalFbo.bindFramebuffer();
        gl::setMatricesWindowPersp(getWindowSize());
        gl::clear( ColorA(0, 0, 0, 0 ) );
        gl::drawSphere(Vec3f(200,100, 0), 6.0);
    mNormalFbo.unbindFramebuffer();
    
    
    
    
    
    
    // 블러 때리는용 Fbo 를 만들어서 또 그린다. (지금은 그냥 오리지널만 써도 될듯.)
    
    mGlowFbo.bindFramebuffer();
        gl::setMatricesWindowPersp(getWindowSize());
        gl::clear( ColorA(0, 0, 0, 0 ) );
        gl::drawSphere(Vec3f(200,100, 0), 6.0);
    mGlowFbo.unbindFramebuffer();
    
    
    // 텍스쳐 블러시킬 Fbo 를 만든다. 텍스쳐가 정면에 나와야 되니까 카메라 세팅을 신더 기본으로 해준다.
    // 블러 때리는용 Fbo 를 텍스쳐화 시켜 그린다.
    //mBlurFbo.bindFramebuffer();
        gl::clear( ColorA(1, 0, 0, 0 ) );
    
        gl::setMatricesWindowPersp(getWindowSize());
    
        mGlowFbo.getTexture().bind(1);  // 얘를 바인딩 해야한다. 그래서 sampler 로 전달하구, 텍스쳐로 그려내야돼.
        mBlurShader.bind();
        mBlurShader.uniform("GlowFboTex", 1);   // glowfbo 를 전사한 텍스쳐를 sampler1d 로 보낸다.
        mBlurShader.uniform("sampleOffset", Vec2f(0.7, 0.3) * ( 3.0f / getWindowWidth() ) );
    
        gl::drawSolidRect( Rectf(0.0, 0.0, getWindowWidth(), getWindowHeight() ) );
        mBlurShader.unbind();
        mGlowFbo.getTexture().unbind();
    //mBlurFbo.unbindFramebuffer();
    
    
    
    // 최종적으로 블러된 Fbo 를 전사한다.
    //gl::draw(mBlurFbo.getTexture());
    

}

CINDER_APP_BASIC( GlowTestApp, RendererGl )
