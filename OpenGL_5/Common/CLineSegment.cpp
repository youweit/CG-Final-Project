#include "CLineSegment.h"
// Example 3 �}�l�令���¤W(Y�b)


CLineSegment::CLineSegment(const vec4 SPoint, const vec4 EPoint, const vec4 vColor)
{
	// �w�]�b X �b�W�� (-1,0,0) �� (1,0,0) ���u�q
	m_Points[0] = SPoint; m_Points[1] = EPoint;

	// �w�]������
	m_Colors[0] = m_Colors[1] = vColor;  // (r, g, b, a)

	// Create and initialize a buffer object 
	CreateBufferObject();

	m_bUpdateMV = false;
	m_bUpdateProj = false;
}


void CLineSegment::CreateBufferObject()
{
    glGenVertexArrays( 1, &m_uiVao );
    glBindVertexArray( m_uiVao );

    // Create and initialize a buffer object

    glGenBuffers( 1, &m_uiBuffer );
    glBindBuffer( GL_ARRAY_BUFFER, m_uiBuffer );
    glBufferData( GL_ARRAY_BUFFER, sizeof(m_Points) + sizeof(m_Colors), NULL, GL_STATIC_DRAW );

    glBufferSubData( GL_ARRAY_BUFFER, 0, sizeof(m_Points), m_Points ); 
	glBufferSubData( GL_ARRAY_BUFFER, sizeof(m_Points), sizeof(m_Colors), m_Colors );
}

void CLineSegment::SetShader(mat4 &mxModelView, mat4 &mxProjection, GLuint uiShaderHandle)
{
    // Load shaders and use the resulting shader program
	if( uiShaderHandle == MAX_UNSIGNED_INT) m_uiProgram = InitShader("vsVtxColor.glsl", "fsVtxColor.glsl");
	else m_uiProgram = uiShaderHandle;

    // set up vertex arrays
    GLuint vPosition = glGetAttribLocation( m_uiProgram, "vPosition" );
    glEnableVertexAttribArray( vPosition );
    glVertexAttribPointer( vPosition, 4, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(0) );

    GLuint vColor = glGetAttribLocation( m_uiProgram, "vColor" ); 
    glEnableVertexAttribArray( vColor );
    glVertexAttribPointer( vColor, 4, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(sizeof(m_Points)) );

	m_uiModelView = glGetUniformLocation( m_uiProgram, "ModelView" );
	m_mxMVFinal = m_mxModelView = mxModelView;
	glUniformMatrix4fv( m_uiModelView, 1, GL_TRUE, m_mxModelView );

    m_uiProjection = glGetUniformLocation( m_uiProgram, "Projection" );
	m_mxProjection = mxProjection;
	glUniformMatrix4fv( m_uiProjection, 1, GL_TRUE, m_mxProjection );
	glBindBuffer( GL_ARRAY_BUFFER, 0 );
}

void CLineSegment::SetModelViewMatrix(mat4 &mat)
{
	m_mxModelView = mat;
	m_bUpdateMV = true;
}

void CLineSegment::SetProjectionMatrix(mat4 &mat)
{
	m_mxProjection = mat;
	m_bUpdateProj = true;
}

void CLineSegment::SetTRSMatrix(mat4 &mat)
{
	m_mxTRS = mat;
	m_bUpdateMV = true;
}

void CLineSegment::SetColor(vec4 vColor)
{
	m_Colors[0] = m_Colors[1] = vColor;
	glBindBuffer( GL_ARRAY_BUFFER, m_uiBuffer );
	glBufferSubData( GL_ARRAY_BUFFER, sizeof(m_Points), sizeof(m_Colors), m_Colors );
}

void CLineSegment::SetVtxColors(vec4 SPColor, vec4 EPColor)
{
	m_Colors[0] = SPColor;
	m_Colors[1] = EPColor;
	glBindBuffer( GL_ARRAY_BUFFER, m_uiBuffer );
	glBufferSubData( GL_ARRAY_BUFFER, sizeof(m_Points), sizeof(m_Colors), m_Colors );
}

void CLineSegment::Draw()
{
	glUseProgram( m_uiProgram );
	glBindVertexArray( m_uiVao );
	if( m_bUpdateMV ) {
		m_mxMVFinal = m_mxModelView * m_mxTRS;
		glUniformMatrix4fv( m_uiModelView, 1, GL_TRUE, m_mxMVFinal );
		m_bUpdateMV = false;
	}
	else glUniformMatrix4fv( m_uiModelView, 1, GL_TRUE, m_mxMVFinal );
	if( m_bUpdateProj ) {
		glUniformMatrix4fv( m_uiProjection, 1, GL_TRUE, m_mxProjection );
		m_bUpdateProj = false;
	}
	glDrawArrays( GL_LINES, 0, VERTEX_NUM );
}

void CLineSegment::DrawW()
{
	glBindVertexArray( m_uiVao );
	if( m_bUpdateMV ) {
		m_mxMVFinal = m_mxModelView * m_mxTRS;
		glUniformMatrix4fv( m_uiModelView, 1, GL_TRUE, m_mxMVFinal );
		m_bUpdateMV = false;
	}
	else glUniformMatrix4fv( m_uiModelView, 1, GL_TRUE, m_mxMVFinal );
	if( m_bUpdateProj ) {
		glUniformMatrix4fv( m_uiProjection, 1, GL_TRUE, m_mxProjection );
		m_bUpdateProj = false;
	}

	glDrawArrays( GL_LINES, 0, VERTEX_NUM );
}
