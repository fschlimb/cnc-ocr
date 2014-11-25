{% import "common_macros.inc.c" as util with context -%}
{{ util.auto_file_banner() }}

{% set defname = "_CNC_" ~ g.name.upper() ~ "_H_INCLUDED_" -%}
#ifndef {{defname}}
#define {{defname}}

#ifdef _DIST_
# include "dist_cnc.h"
#else // _DIST_
# include "cnc.h"
#endif // _DIST_

/*********************************************************************************\
 * tag/step combo collection, needed as long cncocr doesn't support tag-collections
\*********************************************************************************/
template< typename Tag, typename Step >
class tagged_step_collection : public CnC::tag_collection< Tag >, CnC::step_collection< Step >
{
    template< typename Derived >
    tagged_step_collection( CnC::context< Derived > & ctxt, const std::string & name )
      : CnC::tag_collection< Tag >( ctxt, name ), CnC::step_collection< Step >( ctxt, name )
    {}
}


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
{% if i.key|count > 1 -%}
typedef struct {{name}}_key {
    {{name}}_key( const cnc_tag_t {{i.key|join("_=-1, const cnc_tag_t ")}}_=-1 ) {{": "}}
    {%- for arg in i.key -%}
    {{arg ~ "( " ~ arg ~ "_ )" ~ (", " if not loop.last)}}
    {%- endfor -%}
    {{" {}"}}
    cnc_tag_t {{i.key|join(", ")}};
} {{name}}_key_t;

template <> struct cnc_hash< {{name}}_key_t >
{ size_t operator()(const {{name}}_tag_t& tt) const {
    return (
    {%- for arg in i.key -%}
        {{" (tt." ~ arg ~ (")" if loop.first else " << ~ ((3*" ~ loop.index ~ ")-1))") ~ (" +" if not loop.last)}}
    {%- endfor -%}
    {{" );"}}
} };

#ifdef _DIST_
CNC_BITWISE_SERIALIZABLE( {{name}}_key_t );
#endif
{% else -%}

typedef cnc_tag_t {{name}}_key_t;
{% endif %}
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
{% if stepfun.tag|count > 1 -%}
typedef struct {{stepfun.collName}}_tag {
    {{stepfun.collName}}_tag( const cnc_tag_t {{stepfun.tag|join("_=-1, const cnc_tag_t ")}}_=-1 ) {{": "}} 
    {%- for arg in stepfun.tag -%}
    {{arg ~ "( " ~ arg ~ "_ )" ~ (", " if not loop.last)}}
    {%- endfor -%}
    {{" {}"}}
    cnc_tag_t {{stepfun.tag|join(", ")}};
} {{stepfun.collName}}_tag_t;

template <> struct cnc_hash< {{stepfun.collName}}_tag_t >
{ size_t operator()(const {{stepfun.collName}}_tag_t& tt) const {
    return (
    {%- for arg in stepfun.tag -%}
        {{" (tt." ~ arg ~ (")" if loop.first else " << ~ ((3*" ~ loop.index ~ ")-1))") ~ (" +" if not loop.last)}}
    {%- endfor -%}
    {{" );"}}
} };

#ifdef _DIST_
CNC_BITWISE_SERIALIZABLE( {{stepfun.collName}}_tag_t );
#endif
{%- else -%}
typedef cnc_tag_t {{stepfun.collName}}_tag_t;
{%- endif %}

struct {{stepfun.collName}}_step { int execute( const {{stepfun.collName}}_tag_t &, {{g.name}}_context & ) const; }
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
    {%- for i in g.concreteItems %}
    {{ ("CnC::item_collection< " ~ i.collName ~ "_key_t, " ~ i.type ~ " > " ~ i.collName ~ ";") if i.key}}
    {%- endfor %}

    // constructor
    {{g.name}}_context() :
        // initialize tag/step-collections
        {%- for stepfun in g.finalAndSteps -%}
        {%- set isFinalizer = loop.first -%}
        {%- if not isFinalizer %}
        {{stepfun.collName}}( *this, "{{stepfun.collName}}" ),
        {%- endif -%}
        {%- endfor %}
        // initialize item-collections
        {%- for i in g.concreteItems %}
        {%- if i.key %}
        {{"," if not loop.first}} {{i.collName}}( *this, "{{i.collName}}" )
        {%- endif -%}
        {%- endfor %}
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
}

#endif // {{defname}}
