{% import "common_macros.inc.c" as util with context -%}
{{ util.auto_file_banner() }}
{% import "icnc/macros_icnc.inc" as util -%}

{% set defname = "_CNC_" ~ g.name.upper() ~ "_H_INCLUDED_" -%}
#ifndef {{defname}}
#define {{defname}}

#ifdef _DIST_
# include <cnc/dist_cnc.h>
#else // _DIST_
# include <cnc/cnc.h>
#endif // _DIST_

struct {{g.name}}_context;

/*********************************************************************************\
 * tag/step combo collection, needed as long cncocr doesn't support tag-collections
\*********************************************************************************/
template< typename Tag, typename Step >
class tagged_step_collection : public CnC::tag_collection< Tag >, public CnC::step_collection< Step >
{
public:
    template< typename Derived >
    tagged_step_collection( CnC::context< Derived > & ctxt, const std::string & name )
      : CnC::tag_collection< Tag >( ctxt, name ), CnC::step_collection< Step >( ctxt, name )
    {}
};


/*********************************************************************************\
 * type of tag-components
\*********************************************************************************/
typedef int cnc_tag_t;


/*********************************************************************************\
 * tag/key types for items
\*********************************************************************************/
{% for name, i in g.itemDeclarations.items() -%}
// *********************************************************
// item-collection {{name}}
{{util.render_tagdef(name, i.key, 'key')}}
{% endfor %}

/*********************************************************************************\
 * types of control-tags
 * and step declarations
\*********************************************************************************/
{%- for stepfun in g.finalAndSteps %}
{%- set isFinalizer = loop.first -%}
{% if not isFinalizer -%}
// *********************************************************
// Step {{stepfun.collName}}
{{util.render_tagdef(stepfun.collName, stepfun.tag, 'tag')}}
struct {{stepfun.collName}}_step { int execute( const {{stepfun.collName}}_tag_t &, {{g.name}}_context & ) const; };
{% endif %}
{% endfor %}

/*********************************************************************************\
 * CnC context/graph
\*********************************************************************************/
struct {{g.name}}_context : public CnC::context< {{g.name}}_context >
{
    // tag/step-collections
    {% for stepfun in g.finalAndSteps %}
    {%- set isFinalizer = loop.first -%}
    {% if not isFinalizer -%}
    {{"tagged_step_collection< " ~ stepfun.collName ~ "_tag_t, " ~  stepfun.collName ~ "_step > " ~ stepfun.collName ~ ";"}}
    {% endif -%}
    {%- endfor %}

    // item-collections
    {%- for i in g.itemDeclarations %}
    {{ ("CnC::item_collection< " ~ i ~ "_key_t, " ~ g.itemDeclarations[i].type ~ " > " ~ i ~ ";")}}
    {%- endfor %}

    // other parameters
    {%- for line in g.ctxParams %}
    {{ line }}
    {%- endfor %}

    // constructor
    {{g.name}}_context( {{util.render_ctxtargs_param(g, dflt=' = -1')}} ) :
        // initialize tag/step-collections
        {%- for stepfun in g.finalAndSteps -%}
        {%- set isFinalizer = loop.first -%}
        {%- if not isFinalizer %}
        {{stepfun.collName}}( *this, "{{stepfun.collName}}" ),
        {%- endif -%}
        {%- endfor %}
        // initialize item-collections
        {%- for i in g.itemDeclarations %}
        {{"," if not loop.first}} {{i}}( *this, "{{i}}" )
        {%- endfor %}
        // initialize other parameters
        {{util.render_ctxtargs_init(g)}}
    {
        // data and control relationships
        {%- for stepfun in g.finalAndSteps -%}
        {%- set isFinalizer = loop.first -%}
        {%- if not isFinalizer %}
        // {{stepfun.collName}}
        {%- for input in stepfun.inputs %}
        {{stepfun.collName}}.consumes( {{input.collName}} );
        {%- endfor %}
        {%- for output in stepfun.outputs %}
        {{stepfun.collName}}.{{"produces" if output.kind == 'ITEM' else "controls"}}( {{output.collName}} );
        {%- endfor -%}
        {%- endif  -%}
        {%- endfor %}
    }

#ifdef _DIST_
    virtual void serialize( CnC::serializer & ser )
    {
        ser & {{util.render_ctxtargs_param(g, types=False, pfx='', sep=' & ')}};
    }
#endif
 };

#endif // {{defname}}
