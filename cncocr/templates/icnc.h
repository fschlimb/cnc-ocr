{% import "common_macros.inc.c" as util with context -%}
{{ util.auto_file_banner() }}

{% set defname = "_CNC_" ~ g.name.upper() ~ "_H_INCLUDED_" -%}
#ifndef {{defname}}
#define {{defname}}

#include "cnc.h"

/***************************\
 ******** CNC GRAPH ********
\***************************/
{#- /* TODO - Move this section into _external.h */ #}
typedef int cnc_tag_t;

{% for name, i in g.itemDeclarations.items() -%}
{{ "typedef struct { cnc_tag_t " ~ i.key|join(", ") ~ "; } " ~ name ~ "_tag_t;" if i.key}}
{% endfor %}
{% for stepfun in g.finalAndSteps %}
{%- set isFinalizer = loop.first -%}
{% if not isFinalizer -%}
{{ "typedef " ~ (("struct { cnc_tag_t " ~ stepfun.tag|join(", ") ~ "; } " ) if stepfun.tag|count > 1 else "cnc_tag_t " ) ~ stepfun.collName ~ "_tag_t;"}}
{% endif -%}
{% endfor %}

struct {{g.name}}_context : public CnC::context< {{g.name}}_context >
{
    {% for stepfun in g.finalAndSteps %}
    {%- set isFinalizer = loop.first -%}
    {% if not isFinalizer -%}
    {{"tagged_step_collection< " ~ stepfun.collName ~ "_tag_t, " ~  stepfun.collName ~ "_body > " ~ stepfun.collName ~ ";"}}
    {% endif -%}
    {%- endfor %}

    {%- for i in g.concreteItems %}
    {{ ("CnC::item_collection< " ~ i.collName ~ "_tag_t, " ~ i.type ~ " > " ~ i.collName ~ ";") if i.key}}
    {%- endfor %}
}

#endif // {{defname}}
