#ifndef RESET_GL_PROPERTIES_H
#define RESET_GL_PROPERTIES_H

namespace util {

// Implementable class which allows for resetting 
// various OpenGL items to the original state in
// the parent
class IResetGlProperties {
public:
	IResetGlProperties()
	{
	}

	void reset_fbo() const
	{
		reset_fbo_impl();
	}

	void reset_viewport() const
	{
		reset_viewport_impl();
	}
private:
	virtual void reset_fbo_impl() const = 0;
	virtual void reset_viewport_impl() const = 0;

}; // class IResetGlProperties

} // namespace util

#endif // RESET_GL_PROPERTIES_H